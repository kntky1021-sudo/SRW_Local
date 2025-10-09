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
    , selectedUnitId_(-1)
    , isSelectingDestination_(false)
    , isShowingMenu_(false)
    , isSelectingAttackTarget_(false)
    , menuCursor_(0)
{
    // タイルマップ作成
    tileMap_ = std::make_unique<TileMap>(renderer_, 32, 32);

    // カーソル作成
    cursor_ = std::make_unique<Cursor>(renderer_, 32, 32);

    // バトルマネージャー作成
    battleManager_ = std::make_unique<BattleManager>(renderer_);

    // カメラは後で初期化（マップサイズが必要）
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

    // ユニット配置
    // 味方ユニット
    battleManager_->addUnit(2, 2, false, "Gundam");
    battleManager_->addUnit(3, 2, false, "Mazinger");
    battleManager_->addUnit(4, 2, false, "Getter");

    // 敵ユニット（テスト用：1体のみ、HP10）
    battleManager_->addUnit(5, 3, true, "Zaku A");

    // カーソル初期位置（最初の味方ユニットに）
    cursor_->setPosition(2, 2);

    // ユニット選択状態リセット
    selectedUnitId_ = -1;
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
    int unitId = battleManager_->getUnitAt(cx, cy);

    if (unitId < 0) return;  // ユニットがいない

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
    if (battleManager_->isEnemyUnit(unitId)) {
        SDL_SetRenderDrawColor(sdlR, 100, 0, 0, 200);  // 赤系
    }
    else {
        SDL_SetRenderDrawColor(sdlR, 0, 0, 100, 200);  // 青系
    }
    SDL_RenderFillRect(sdlR, &panelRect);

    // 枠線
    SDL_SetRenderDrawColor(sdlR, 255, 255, 255, 255);
    SDL_RenderRect(sdlR, &panelRect);

    // HPバー（簡易版）
    auto pos = battleManager_->getUnitPosition(unitId);

    // ユニット名表示位置（テキストの代わりにマーカー）
    SDL_FRect nameMarker{
        panelRect.x + 10,
        panelRect.y + 10,
        10.0f,
        10.0f
    };

    if (battleManager_->isEnemyUnit(unitId)) {
        SDL_SetRenderDrawColor(sdlR, 255, 0, 0, 255);
    }
    else {
        SDL_SetRenderDrawColor(sdlR, 0, 0, 255, 255);
    }
    SDL_RenderFillRect(sdlR, &nameMarker);

    // TODO: 実際のユニット名とHPをフォントで描画
    // 現状は色で敵味方を区別するのみ
}

void GameManager::selectUnit() {
    int cx = cursor_->getX();
    int cy = cursor_->getY();

    // カーソル位置にユニットがいるかチェック
    int unitId = battleManager_->getUnitAt(cx, cy);

    if (unitId >= 0) {
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

        std::cout << "[GameManager] Unit " << battleManager_->getUnitName(unitId)
            << " (ID:" << unitId << ") selected at ("
            << cx << ", " << cy << ")\n";

        // 移動可能範囲を計算（仮：周囲5マス）
        movableArea_.clear();
        for (int dy = -5; dy <= 5; ++dy) {
            for (int dx = -5; dx <= 5; ++dx) {
                if (std::abs(dx) + std::abs(dy) <= 5) {
                    int nx = cx + dx;
                    int ny = cy + dy;
                    if (nx >= 0 && ny >= 0 &&
                        nx < tileMap_->getMapWidth() &&
                        ny < tileMap_->getMapHeight()) {
                        // 他のユニットがいない場所のみ
                        if (battleManager_->getUnitAt(nx, ny) < 0) {
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
        std::cout << "[GameManager] No unit at cursor position\n";
    }
}

void GameManager::cancelSelection() {
    if (isSelectingDestination_) {
        std::cout << "[GameManager] Move cancelled\n";
        selectedUnitId_ = -1;
        isSelectingDestination_ = false;
        movableArea_.clear();
    }
}

void GameManager::confirmMove() {
    if (selectedUnitId_ < 0) return;

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
        battleManager_->moveUnit(selectedUnitId_, destX, destY);
        std::cout << "[GameManager] Unit " << battleManager_->getUnitName(selectedUnitId_)
            << " (ID:" << selectedUnitId_ << ") moved to (" << destX << ", " << destY << ")\n";

        // 移動可能範囲クリア
        movableArea_.clear();
        isSelectingDestination_ = false;

        // 行動済みフラグを立てる
        battleManager_->setUnitActed(selectedUnitId_, true);

        // 少し待機してからメニュー表示（キー入力のクリア）
        SDL_Delay(100);

        // selectedUnitId_は保持したままメニュー表示
        showUnitMenu();
    }
    else {
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

    // 武器選択UI作成
    WeaponSelectUI weaponUI(renderer_, ui_);

    // テスト用武器リスト作成
    std::vector<WeaponData> weapons;

    // テスト武器1
    WeaponData weapon1;
    weapon1.name = "Beam Rifle";
    weapon1.power = 2000;
    weapon1.minRange = 2;
    weapon1.maxRange = 5;
    weapon1.ammo = 8;
    weapon1.enCost = 10;
    weapon1.isMap = false;
    weapon1.attribute = "Beam";
    weapons.push_back(weapon1);

    // テスト武器2
    WeaponData weapon2;
    weapon2.name = "Beam Saber";
    weapon2.power = 2800;
    weapon2.minRange = 1;
    weapon2.maxRange = 1;
    weapon2.ammo = -1;  // 無限
    weapon2.enCost = 15;
    weapon2.isMap = false;
    weapon2.attribute = "Melee";
    weapons.push_back(weapon2);

    // テスト武器3
    WeaponData weapon3;
    weapon3.name = "Vulcan";
    weapon3.power = 800;
    weapon3.minRange = 1;
    weapon3.maxRange = 3;
    weapon3.ammo = 30;
    weapon3.enCost = 0;
    weapon3.isMap = false;
    weapon3.attribute = "Physical";
    weapons.push_back(weapon3);

    // 武器選択（EN=100、距離=1と仮定）
    int weaponIndex = weaponUI.selectWeapon(weapons, 100, 1);

    if (weaponIndex < 0) {
        // キャンセル→メニューに戻る
        showUnitMenu();
        return;
    }

    std::cout << "[GameManager] Selected weapon: " << weapons[weaponIndex].name << "\n";

    // 攻撃対象選択へ
    isSelectingAttackTarget_ = true;

    // 攻撃可能範囲を計算（周囲1マス）
    auto pos = battleManager_->getUnitPosition(selectedUnitId_);
    int ux = pos[0];
    int uy = pos[1];

    attackableArea_.clear();

    // 上下左右をチェック
    const int dx[] = { 0, 0, -1, 1 };
    const int dy[] = { -1, 1, 0, 0 };

    for (int i = 0; i < 4; ++i) {
        int nx = ux + dx[i];
        int ny = uy + dy[i];

        if (nx >= 0 && ny >= 0 &&
            nx < tileMap_->getMapWidth() &&
            ny < tileMap_->getMapHeight()) {
            // 敵ユニットがいる場所のみ
            int targetId = battleManager_->getUnitAt(nx, ny);

            if (targetId >= 0 && battleManager_->isEnemyUnit(targetId)) {
                attackableArea_.push_back({ nx, ny });
                std::cout << "[GameManager] Target: "
                    << battleManager_->getUnitName(targetId)
                    << " at (" << nx << ", " << ny << ")\n";
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
    std::cout << "[GameManager] Unit " << battleManager_->getUnitName(selectedUnitId_)
        << " is waiting\n";
    // 行動済みフラグは移動時に既にセット済み

    // 選択をリセット
    selectedUnitId_ = -1;
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
            int targetId = battleManager_->getUnitAt(cx, cy);

            // 攻撃可能範囲内かチェック
            bool canAttack = false;
            for (const auto& p : attackableArea_) {
                if (p.first == cx && p.second == cy) {
                    canAttack = true;
                    break;
                }
            }

            if (canAttack && targetId >= 0) {
                executeAttack(targetId);
                keyPressed = true;
                return;  // 攻撃実行したら即リターン
            }
            else {
                // デバッグ情報は削除（攻撃は実行されるが、エラーが出るのは誤検知）
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
    isSelectingAttackTarget_ = false;
    attackableArea_.clear();

    auto attackerPos = battleManager_->getUnitPosition(selectedUnitId_);
    auto defenderPos = battleManager_->getUnitPosition(targetId);

    std::cout << "[GameManager] " << battleManager_->getUnitName(selectedUnitId_)
        << " attacks " << battleManager_->getUnitName(targetId) << "!\n";

    // 簡易ユニットオブジェクトの作成（仮実装）
    // TODO: 将来的にはBattleManagerから完全なUnitオブジェクトを取得
    Unit attackerUnit(
        battleManager_->getUnitName(selectedUnitId_),
        Team::Ally,
        'P',
        battleManager_->getUnitMaxHP(selectedUnitId_),
        5,  // moveRange
        1,  // attackRange
        100,  // attackPower
        50,  // defensePower
        100   // speed
    );
    attackerUnit.setPosition(attackerPos[0], attackerPos[1]);
    // 現在のHPを設定（簡易版）
    int currentHp = battleManager_->getUnitHP(selectedUnitId_);
    int maxHp = battleManager_->getUnitMaxHP(selectedUnitId_);
    if (currentHp < maxHp) {
        attackerUnit.takeDamage(maxHp - currentHp);
    }

    Unit defenderUnit(
        battleManager_->getUnitName(targetId),
        Team::Enemy,
        'E',
        battleManager_->getUnitMaxHP(targetId),
        5,
        1,
        50,
        30,
        80
    );
    defenderUnit.setPosition(defenderPos[0], defenderPos[1]);
    currentHp = battleManager_->getUnitHP(targetId);
    maxHp = battleManager_->getUnitMaxHP(targetId);
    if (currentHp < maxHp) {
        defenderUnit.takeDamage(maxHp - currentHp);
    }

    // BattleSceneで戦闘実行
    BattleScene battleScene(ui_, renderer_);
    BattleResult result = battleScene.performBattle(
        &attackerUnit,
        &defenderUnit,
        nullptr,  // 武器は後で実装
        'L'       // 地形
    );

    // 結果をBattleManagerに反映
    battleManager_->applyDamage(targetId, result.damage);

    // 経験値付与
    battleManager_->addExperience(selectedUnitId_, result.expGained);

    // 選択をリセット
    selectedUnitId_ = -1;

    // 元の画面に戻る
    render();
    SDL_Delay(500);

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