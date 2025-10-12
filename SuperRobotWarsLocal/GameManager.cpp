#include "GameManager.h"
#include <SDL.h>
#include <iostream>
#include "BattleScene.h"
#include "Unit.h"          // ← 追加
#include "WeaponSelectUI.h"
#include "UnitData.h"

GameManager::GameManager(
    UIManager* ui,
    InputManager* input,
    SDLRenderer* renderer,
    int windowW,
    int windowH)
    : ui_(ui)
    , input_(input)
    , renderer_(renderer)
    , state_(GameState::MainMenu)
    , windowW_(windowW)
    , windowH_(windowH)
    , currentTurn_(1)
    , selectedUnitId_("")
    , isSelectingDestination_(false)
    , isShowingMenu_(false)
    , isSelectingAttackTarget_(false)
    , menuCursor_(0)
    , audioManager_(nullptr)  // ← これを追加
{
    // UnitDatabase初期化
    std::cout << "[GameManager] Loading unit database...\n";
    if (!unitDB_.loadRobots("data/robots.json")) {
        std::cerr << "[GameManager] Failed to load robots.json\n";
    }
    if (!unitDB_.loadPilots("data/pilots.json")) {
        std::cerr << "[GameManager] Failed to load pilots.json\n";
    }

    // タイルマップ作成
    tileMap_ = std::make_unique<TileMap>(renderer_, 32, 32);

    // カーソル作成
    cursor_ = std::make_unique<Cursor>(renderer_, 32, 32);

    // バトルマネージャー作成
    battleManager_ = std::make_unique<BattleManager>(renderer_);
}

GameManager::~GameManager() = default;

void GameManager::run() {
    std::cout << "[GameManager] Game started\n";

    // デバッグ用：直接バトル開始
    startBattle("maps/tileset.bmp");
    state_ = GameState::PlayerTurn;

    // メインループ
    while (state_ != GameState::Exit) {
        input_->pollEvents();

        if (input_->quitRequested()) {
            state_ = GameState::Exit;
            break;
        }

        // 状態別の更新処理
        switch (state_) {
        case GameState::MainMenu:
            updateMainMenu();
            break;

        case GameState::StageSelect:
            updateStageSelect();
            break;

        case GameState::PlayerTurn:
            updatePlayerTurn();
            break;

        case GameState::EnemyTurn:
            updateEnemyTurn();
            break;

        case GameState::BattleResult:
            updateBattleResult();
            break;

        case GameState::GameOver:
            // ゲームオーバー画面
            ui_->showMessage("=== GAME OVER ===\nPress any key to exit");
            input_->waitKey();
            state_ = GameState::Exit;
            break;

        default:
            break;
        }

        // 描画
        render();

        SDL_Delay(16);  // 約60FPS
    }

    std::cout << "[GameManager] Game ended\n";
}

void GameManager::startBattle(const std::string& mapPath) {
    std::cout << "[GameManager] Starting battle: " << mapPath << "\n";

    if (audioManager_) {
        audioManager_->playBGM("map", true, 1000);  // マップBGM（1秒フェードイン）
    }

    // マップ読み込み
    if (!tileMap_->loadFromFile(mapPath)) {
        std::cerr << "[GameManager] Failed to load map\n";
        state_ = GameState::Exit;
        return;
    }

    // カメラ初期化
    camera_ = std::make_unique<Camera>(
        tileMap_->getMapWidth() * tileMap_->getTileWidth(),
        tileMap_->getMapHeight() * tileMap_->getTileHeight(),
        windowW_,
        windowH_
    );

    // バトルマネージャー初期化
    battleManager_->startBattle(mapPath);

    // ユニット配置（新方式）
    // 味方ユニット
    auto gundam = unitDB_.createUnit("player_gundam", "gundam_rx78", "amuro_ray", Team::Ally);
    if (gundam) {
        battleManager_->registerUnit(std::move(gundam), 2, 2);
    }

    auto mazinger = unitDB_.createUnit("player_mazinger", "mazinger_z", "kouji_kabuto", Team::Ally);
    if (mazinger) {
        battleManager_->registerUnit(std::move(mazinger), 3, 2);
    }

    auto getter = unitDB_.createUnit("player_getter", "getter_robo", "ryoma_nagare", Team::Ally);
    if (getter) {
        battleManager_->registerUnit(std::move(getter), 4, 2);
    }

    // 敵ユニット
    auto zaku1 = unitDB_.createUnit("enemy_zaku_1", "zaku_ii", "zaku_pilot", Team::Enemy);
    if (zaku1) {
        battleManager_->registerUnit(std::move(zaku1), 5, 3);
    }

    // デバッグ：全ユニット表示
    battleManager_->printAllUnits();

    // カーソル初期位置
    cursor_->setPosition(2, 2);

    // ユニット選択状態リセット
    selectedUnitId_ = "";
    isSelectingDestination_ = false;
    isShowingMenu_ = false;
    isSelectingAttackTarget_ = false;
    menuCursor_ = 0;
    movableArea_.clear();
    attackableArea_.clear();

    // ターン初期化
    currentTurn_ = 1;

    state_ = GameState::InBattle;
}

void GameManager::updateMainMenu() {
    // TODO: メインメニューの実装
    state_ = GameState::StageSelect;
}

void GameManager::updateStageSelect() {
    // TODO: ステージ選択の実装
    startBattle("maps/tileset.bmp");
    state_ = GameState::PlayerTurn;
}

void GameManager::updatePlayerTurn() {
    // メニュー表示中の処理
    if (isShowingMenu_) {
        handleMenuInput();
        return;
    }

    // 攻撃対象選択中の処理
    if (isSelectingAttackTarget_) {
        handleAttackTargetSelection();
        return;
    }

    // 通常の入力処理
    const bool* keyState = SDL_GetKeyboardState(nullptr);

    static bool keyPressed = false;

    if (!keyPressed) {
        // Zキー：決定（ユニット選択/移動確定）
        if (keyState[SDL_SCANCODE_Z]) {
            if (!isSelectingDestination_) {
                // ユニット選択
                selectUnit();
            }
            else {
                // 移動確定
                confirmMove();
            }
            keyPressed = true;
        }
        // Xキー：キャンセル
        else if (keyState[SDL_SCANCODE_X]) {
            cancelSelection();
            keyPressed = true;
        }
        // カーソル移動
        else {
            int dx = 0, dy = 0;

            if (keyState[SDL_SCANCODE_UP]) {
                dy = -1;
                keyPressed = true;
            }
            else if (keyState[SDL_SCANCODE_DOWN]) {
                dy = +1;
                keyPressed = true;
            }
            else if (keyState[SDL_SCANCODE_LEFT]) {
                dx = -1;
                keyPressed = true;
            }
            else if (keyState[SDL_SCANCODE_RIGHT]) {
                dx = +1;
                keyPressed = true;
            }

            if (dx != 0 || dy != 0) {
                cursor_->move(dx, dy,
                    tileMap_->getMapWidth(),
                    tileMap_->getMapHeight());
            }
        }

        // スペースキーでターン終了
        if (keyState[SDL_SCANCODE_SPACE]) {
            std::cout << "[GameManager] Player turn ended\n";

            // ターン終了前に勝利条件チェック
            if (checkVictoryCondition()) {
                state_ = GameState::BattleResult;
            }
            else {
                state_ = GameState::EnemyTurn;
            }
            keyPressed = true;
        }

        // ESCキーで終了
        if (keyState[SDL_SCANCODE_ESCAPE]) {
            std::cout << "[GameManager] Exiting game\n";
            state_ = GameState::Exit;
            keyPressed = true;
        }
    }

    // キーが離されたらリセット
    if (!keyState[SDL_SCANCODE_UP] &&
        !keyState[SDL_SCANCODE_DOWN] &&
        !keyState[SDL_SCANCODE_LEFT] &&
        !keyState[SDL_SCANCODE_RIGHT] &&
        !keyState[SDL_SCANCODE_Z] &&
        !keyState[SDL_SCANCODE_X] &&
        !keyState[SDL_SCANCODE_SPACE] &&
        !keyState[SDL_SCANCODE_ESCAPE]) {
        keyPressed = false;
    }
}

void GameManager::updateEnemyTurn() {
    std::cout << "[GameManager] === Enemy Turn " << currentTurn_ << " ===\n";

    if (audioManager_) {
        audioManager_->playSE("decide");
    }

    // 簡易的な敵ターン処理
    // TODO: 実際の敵AI実装
    SDL_Delay(1000);

    // 勝敗判定（敵の攻撃後）
    if (checkDefeatCondition()) {
        std::cout << "[GameManager] Game Over\n";
        state_ = GameState::GameOver;
        return;
    }

    // ターン終了して次のプレイヤーターンへ
    endTurn();
    currentTurn_++;

    // プレイヤーターン開始時に味方の行動フラグをリセット
    battleManager_->resetAllActedFlags(false);

    state_ = GameState::PlayerTurn;
    std::cout << "[GameManager] === Player Turn " << currentTurn_ << " ===\n";

    // 勝利判定（ターン開始時）
    if (checkVictoryCondition()) {
        std::cout << "[GameManager] Mission Complete!\n";
        state_ = GameState::BattleResult;
    }
}

void GameManager::updateBattleResult() {
    // 勝利画面表示
    ui_->showMessage("=== MISSION COMPLETE ===\nPress any key to exit");
    input_->waitKey();
    state_ = GameState::Exit;
}

void GameManager::render() {
    // 画面クリア
    SDL_SetRenderDrawColor(renderer_->getSDLRenderer(), 0, 0, 0, 255);
    renderer_->clear();

    switch (state_) {
    case GameState::MainMenu:
        renderMainMenu();
        break;

    case GameState::PlayerTurn:
    case GameState::EnemyTurn:
    case GameState::InBattle:
        renderBattle();
        break;

    default:
        break;
    }

    renderer_->present();
}

void GameManager::renderMainMenu() {
    // TODO: メインメニュー描画
}

void GameManager::renderBattle() {
    // カメラ更新
    camera_->update(
        cursor_->getX() * tileMap_->getTileWidth(),
        cursor_->getY() * tileMap_->getTileHeight()
    );

    int offsetX = camera_->getOffsetX();
    int offsetY = camera_->getOffsetY();

    // マップ描画
    tileMap_->render(offsetX, offsetY);

    // 移動可能範囲の表示（緑色）
    if (isSelectingDestination_ && !movableArea_.empty()) {
        auto* sdlR = renderer_->getSDLRenderer();
        SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdlR, 0, 255, 0, 100);

        for (const auto& pos : movableArea_) {
            SDL_FRect rect{
                static_cast<float>(pos.first * 32 - offsetX),
                static_cast<float>(pos.second * 32 - offsetY),
                32.0f,
                32.0f
            };
            SDL_RenderFillRect(sdlR, &rect);
        }
    }

    // 攻撃可能範囲の表示（赤色）
    if (isSelectingAttackTarget_ && !attackableArea_.empty()) {
        auto* sdlR = renderer_->getSDLRenderer();
        SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdlR, 255, 0, 0, 150);

        for (const auto& pos : attackableArea_) {
            SDL_FRect rect{
                static_cast<float>(pos.first * 32 - offsetX),
                static_cast<float>(pos.second * 32 - offsetY),
                32.0f,
                32.0f
            };
            SDL_RenderFillRect(sdlR, &rect);
        }
    }

    // ユニット描画
    battleManager_->renderUnits(renderer_, offsetX, offsetY);

    // カーソル描画
    cursor_->render(offsetX, offsetY);

    // UI描画（最前面）
    renderUI();
}

void GameManager::startPlayerTurnPhase() {
    currentTurn_++;
    std::cout << "[GameManager] Turn " << currentTurn_ << " - Player Phase\n";
}

void GameManager::startEnemyTurnPhase() {
    std::cout << "[GameManager] Turn " << currentTurn_ << " - Enemy Phase\n";
}

void GameManager::endTurn() {
    std::cout << "[GameManager] Turn " << currentTurn_ << " ended\n";
}

bool GameManager::checkVictoryCondition() {
    // 敵ユニットが全滅したか
    int aliveEnemies = battleManager_->getAliveEnemyCount();

    if (aliveEnemies == 0) {
        std::cout << "[GameManager] === VICTORY ===\n";
        if (audioManager_) {
            audioManager_->stopBGM(1000);  // BGMフェードアウト
            // audioManager_->playSE("victory");  // 勝利SE（追加する場合）
        }
        std::cout << "[GameManager] All enemies defeated!\n";
        return true;
    }

    return false;
}

bool GameManager::checkDefeatCondition() {
    // 味方ユニットが全滅したか
    int aliveAllies = battleManager_->getAliveAllyCount();

    if (aliveAllies == 0) {
        std::cout << "[GameManager] === DEFEAT ===\n";
        std::cout << "[GameManager] All allies destroyed!\n";
        if (audioManager_) {
            audioManager_->stopBGM(1000);
            // audioManager_->playSE("gameover");  // 敗北SE（追加する場合）
        }
        return true;
    }

    return false;
}

void GameManager::renderUI() {
    auto* sdlR = renderer_->getSDLRenderer();

    // 背景バー（上部）
    SDL_FRect bgRect{ 0.0f, 0.0f, static_cast<float>(windowW_), 60.0f };
    SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdlR, 0, 0, 0, 180);
    SDL_RenderFillRect(sdlR, &bgRect);

    // 枠線
    SDL_SetRenderDrawColor(sdlR, 255, 255, 255, 255);
    SDL_RenderRect(sdlR, &bgRect);

    // ターン数表示（テキストの代わりに簡易的な表示）
    // TODO: フォント描画実装後に改善

    // カーソル位置のユニット情報表示
    renderUnitInfo();
}

void GameManager::renderUnitInfo() {
    int cx = cursor_->getX();
    int cy = cursor_->getY();
    Unit* unit = battleManager_->getUnitAt(cx, cy);

    if (!unit) return;

    auto* sdlR = renderer_->getSDLRenderer();

    // 情報パネル（右下）
    SDL_FRect panelRect{
        static_cast<float>(windowW_ - 200),
        static_cast<float>(windowH_ - 80),
        200.0f,
        80.0f
    };

    SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_BLEND);

    // 敵味方で色分け
    if (unit->getTeam() == Team::Enemy) {
        SDL_SetRenderDrawColor(sdlR, 100, 0, 0, 200);
    }
    else {
        SDL_SetRenderDrawColor(sdlR, 0, 0, 100, 200);
    }
    SDL_RenderFillRect(sdlR, &panelRect);

    // 枠線
    SDL_SetRenderDrawColor(sdlR, 255, 255, 255, 255);
    SDL_RenderRect(sdlR, &panelRect);

    // TODO: ユニット名とHPをフォントで描画
    // 現状は色で敵味方を区別するのみ
}

void GameManager::selectUnit() {
    int cx = cursor_->getX();
    int cy = cursor_->getY();

    // カーソル位置にユニットがいるかチェック
    Unit* unit = battleManager_->getUnitAt(cx, cy);

    if (unit) {
        std::string unitId = unit->getId();

        if (audioManager_) {
            audioManager_->playSE("decide");
        }

        // 敵ユニットは選択できない
        if (battleManager_->isEnemyUnit(unitId)) {
            std::cout << "[GameManager] Cannot select enemy unit\n";
            return;
        }

        // 行動済みユニットは選択できない
        if (battleManager_->hasUnitActed(unitId)) {
            std::cout << "[GameManager] Unit has already acted\n";
            return;
        }

        selectedUnitId_ = unitId;
        isSelectingDestination_ = true;

        std::cout << "[GameManager] Unit " << unitId
            << " selected at (" << cx << ", " << cy << ")\n";

        // 移動可能範囲を計算
        int moveRange = unit->getMoveRange();
        movableArea_.clear();
        for (int dy = -moveRange; dy <= moveRange; ++dy) {
            for (int dx = -moveRange; dx <= moveRange; ++dx) {
                if (std::abs(dx) + std::abs(dy) <= moveRange) {
                    int nx = cx + dx;
                    int ny = cy + dy;
                    if (nx >= 0 && ny >= 0 &&
                        nx < tileMap_->getMapWidth() &&
                        ny < tileMap_->getMapHeight()) {
                        // 他のユニットがいない場所のみ
                        if (!battleManager_->getUnitAt(nx, ny)) {
                            movableArea_.push_back({ nx, ny });
                        }
                    }
                }
            }
        }

        std::cout << "[GameManager] Movable area: " << movableArea_.size()
            << " tiles\n";
    }
    else {
        // エラー音
        if (audioManager_) {
            audioManager_->playSE("cancel");
        }
        std::cout << "[GameManager] No unit at cursor position\n";
    }
}

void GameManager::cancelSelection() {
    if (isSelectingDestination_) {
        if (audioManager_) {
            audioManager_->playSE("cancel");
        }
        std::cout << "[GameManager] Move cancelled\n";
        selectedUnitId_ = "";
        isSelectingDestination_ = false;
        movableArea_.clear();
    }
}

void GameManager::confirmMove() {
    if (selectedUnitId_.empty()) return;

    int destX = cursor_->getX();
    int destY = cursor_->getY();

    // 移動可能範囲内かチェック
    bool canMove = false;
    for (const auto& pos : movableArea_) {
        if (pos.first == destX && pos.second == destY) {
            canMove = true;
            break;
        }
    }

    if (canMove) {
        // ユニット移動
                // 移動音
        if (audioManager_) {
            audioManager_->playSE("decide");
        }
        battleManager_->moveUnit(selectedUnitId_, destX, destY);
        std::cout << "[GameManager] Unit " << selectedUnitId_
            << " moved to (" << destX << ", " << destY << ")\n";

        // 移動可能範囲クリア
        movableArea_.clear();
        isSelectingDestination_ = false;

        // 行動済みフラグを立てる
        battleManager_->setUnitActed(selectedUnitId_, true);

        // 少し待機してからメニュー表示
        SDL_Delay(100);

        showUnitMenu();
    }
    else {
        if (audioManager_) {
            audioManager_->playSE("cancel");
        }
        std::cout << "[GameManager] Cannot move to (" << destX << ", "
            << destY << ")\n";
    }
}

void GameManager::showUnitMenu() {
    isShowingMenu_ = true;
    menuCursor_ = 0;
    std::cout << "[GameManager] === Unit Menu ===\n";
    std::cout << "  > [0] Attack\n";
    std::cout << "    [1] Wait\n";
    std::cout << "Use UP/DOWN to select, Z to confirm, X to cancel\n";
}

void GameManager::handleMenuInput() {
    const bool* keyState = SDL_GetKeyboardState(nullptr);
    static bool keyPressed = false;

    if (!keyPressed) {
        // 上下でメニューカーソル移動
        if (keyState[SDL_SCANCODE_UP]) {
            menuCursor_ = (menuCursor_ - 1 + 2) % 2;
            std::cout << "[GameManager] Menu: "
                << (menuCursor_ == 0 ? "> Attack / Wait" : "Attack / > Wait") << "\n";
            keyPressed = true;
        }
        else if (keyState[SDL_SCANCODE_DOWN]) {
            menuCursor_ = (menuCursor_ + 1) % 2;
            std::cout << "[GameManager] Menu: "
                << (menuCursor_ == 0 ? "> Attack / Wait" : "Attack / > Wait") << "\n";
            keyPressed = true;
        }
        // Zキーで決定
        else if (keyState[SDL_SCANCODE_Z]) {
            std::cout << "[GameManager] Selected: "
                << (menuCursor_ == 0 ? "Attack" : "Wait") << "\n";
            if (menuCursor_ == 0) {
                selectAttack();
            }
            else {
                selectWait();
            }
            keyPressed = true;
        }
        // Xキーでキャンセル
        else if (keyState[SDL_SCANCODE_X]) {
            isShowingMenu_ = false;
            std::cout << "[GameManager] Menu cancelled\n";
            keyPressed = true;
        }
    }

    if (!keyState[SDL_SCANCODE_UP] &&
        !keyState[SDL_SCANCODE_DOWN] &&
        !keyState[SDL_SCANCODE_Z] &&
        !keyState[SDL_SCANCODE_X]) {
        keyPressed = false;
    }
}

void GameManager::selectAttack() {
    isShowingMenu_ = false;

    Unit* attacker = battleManager_->getUnitById(selectedUnitId_);
    if (!attacker) {
        std::cerr << "[GameManager] Attacker not found\n";
        return;
    }

    // 武器選択UI作成
    WeaponSelectUI weaponUI(renderer_, ui_);

    // ユニットの実際の武器を取得
    const auto& weapons = attacker->getWeapons();

    if (weapons.empty()) {
        std::cout << "[GameManager] No weapons available\n";
        selectWait();
        return;
    }

    // 武器選択（EN=現在のEN、距離=1と仮定）
    int weaponIndex = weaponUI.selectWeapon(
        weapons,
        attacker->getCurrentEN(),
        1  // TODO: 実際の距離計算
    );

    if (weaponIndex < 0) {
        // キャンセル→メニューに戻る
        showUnitMenu();
        return;
    }

    std::cout << "[GameManager] Selected weapon: " << weapons[weaponIndex].name << "\n";

    // 攻撃可能範囲を計算
    isSelectingAttackTarget_ = true;

    int ux = attacker->getX();
    int uy = attacker->getY();

    attackableArea_.clear();

    // 武器の射程を使用
    const WeaponData* weapon = attacker->getWeapon(weaponIndex);
    if (!weapon) {
        selectWait();
        return;
    }

    int minRange = weapon->minRange;
    int maxRange = weapon->maxRange;

    // 射程内の敵を検索
    for (int dy = -maxRange; dy <= maxRange; ++dy) {
        for (int dx = -maxRange; dx <= maxRange; ++dx) {
            int dist = std::abs(dx) + std::abs(dy);
            if (dist >= minRange && dist <= maxRange) {
                int nx = ux + dx;
                int ny = uy + dy;

                if (nx >= 0 && ny >= 0 &&
                    nx < tileMap_->getMapWidth() &&
                    ny < tileMap_->getMapHeight()) {

                    Unit* target = battleManager_->getUnitAt(nx, ny);
                    if (target && battleManager_->isEnemyUnit(target->getId())) {
                        attackableArea_.push_back({ nx, ny });
                        std::cout << "[GameManager] Target: "
                            << target->getId()
                            << " at (" << nx << ", " << ny << ")\n";
                    }
                }
            }
        }
    }

    if (attackableArea_.empty()) {
        std::cout << "[GameManager] No targets in range. Auto wait.\n";
        isSelectingAttackTarget_ = false;
        selectWait();
    }
    else {
        std::cout << "[GameManager] " << attackableArea_.size()
            << " target(s) available\n";
    }
}

void GameManager::selectWait() {
    isShowingMenu_ = false;
    if (!selectedUnitId_.empty()) {
        std::cout << "[GameManager] Unit " << selectedUnitId_ << " is waiting\n";
        selectedUnitId_ = "";
    }
}

void GameManager::handleAttackTargetSelection() {
    const bool* keyState = SDL_GetKeyboardState(nullptr);
    static bool keyPressed = false;

    if (!keyPressed) {
        // カーソル移動
        int dx = 0, dy = 0;
        if (keyState[SDL_SCANCODE_UP]) {
            dy = -1;
            keyPressed = true;
        }
        else if (keyState[SDL_SCANCODE_DOWN]) {
            dy = +1;
            keyPressed = true;
        }
        else if (keyState[SDL_SCANCODE_LEFT]) {
            dx = -1;
            keyPressed = true;
        }
        else if (keyState[SDL_SCANCODE_RIGHT]) {
            dx = +1;
            keyPressed = true;
        }

        if (dx != 0 || dy != 0) {
            cursor_->move(dx, dy,
                tileMap_->getMapWidth(),
                tileMap_->getMapHeight());
        }

        // Zキーで攻撃実行
        if (keyState[SDL_SCANCODE_Z]) {
            int cx = cursor_->getX();
            int cy = cursor_->getY();
            Unit* target = battleManager_->getUnitAt(cx, cy);  // Unit* に変更

            // 攻撃可能範囲内かチェック
            bool canAttack = false;
            for (const auto& p : attackableArea_) {
                if (p.first == cx && p.second == cy) {
                    canAttack = true;
                    break;
                }
            }

            if (canAttack && target) {
                executeAttack(0);  // 引数は使われない
                keyPressed = true;
                return;
            }
            keyPressed = true;
        }
        // Xキーでキャンセル
        else if (keyState[SDL_SCANCODE_X]) {
            isSelectingAttackTarget_ = false;
            attackableArea_.clear();
            std::cout << "[GameManager] Attack cancelled\n";
            showUnitMenu();
            keyPressed = true;
        }
    }

    if (!keyState[SDL_SCANCODE_UP] &&
        !keyState[SDL_SCANCODE_DOWN] &&
        !keyState[SDL_SCANCODE_LEFT] &&
        !keyState[SDL_SCANCODE_RIGHT] &&
        !keyState[SDL_SCANCODE_Z] &&
        !keyState[SDL_SCANCODE_X]) {
        keyPressed = false;
    }
}

void GameManager::executeAttack(int targetId) {
    // この引数は使わない（旧方式の名残）
    (void)targetId;

    isSelectingAttackTarget_ = false;
    attackableArea_.clear();

    int cx = cursor_->getX();
    int cy = cursor_->getY();

    Unit* defender = battleManager_->getUnitAt(cx, cy);
    if (!defender) {
        std::cerr << "[GameManager] No unit at target position\n";
        selectedUnitId_ = "";
        return;
    }

    Unit* attacker = battleManager_->getUnitById(selectedUnitId_);
    if (!attacker) {
        std::cerr << "[GameManager] Attacker not found\n";
        selectedUnitId_ = "";
        return;
    }

    std::cout << "[GameManager] " << attacker->getId()
        << " attacks " << defender->getId() << "!\n";

    if (audioManager_) {
        audioManager_->playBGM("battle", false);  // 戦闘BGM（ループなし）
    }

    // BattleSceneで戦闘実行
    BattleScene battleScene(ui_, renderer_);
    BattleResult result = battleScene.performBattle(
        attacker,
        defender,
        attacker->getWeapon(0),  // TODO: 選択した武器を使用
        'L'  // 地形
    );

    // 結果をBattleManagerに反映（既にBattleSceneで反映済み）
    // 経験値付与
    battleManager_->addExperience(selectedUnitId_, result.expGained);

    // 選択をリセット
    selectedUnitId_ = "";

    // 元の画面に戻る
    render();
    SDL_Delay(500);

    if (audioManager_) {
        audioManager_->playBGM("map", true, 500);  // 0.5秒フェードイン
    }

    // 攻撃後に勝利条件チェック
    if (checkVictoryCondition()) {
        std::cout << "[GameManager] Victory achieved after attack!\n";
        state_ = GameState::BattleResult;
    }
}

// 戦闘アニメーション用の補助メソッド（空実装）
void GameManager::playBattleAnimation(int attackerId, int defenderId) {
    // TODO: 
    // 1. 戦闘画面への切り替え
    // 2. ユニットグラフィックの表示
    // 3. 攻撃アニメーション
    // 4. ダメージエフェクト
    // 5. 元の画面に戻る
    std::cout << "[GameManager] Battle animation (not implemented)\n";
}

// 戦闘結果の表示（空実装）
void GameManager::showBattleResultDialog(const BattleResult& result) {
    std::stringstream ss;
    ss << "与ダメージ: " << result.damage;

    if (result.isCritical) {
        ss << " (クリティカル!)";
    }

    ss << "\n経験値: +" << result.expGained;

    if (result.defenderDestroyed) {
        ss << "\n敵機撃破！";
    }

    ui_->showMessage(ss.str());
    SDL_Delay(2000);
}