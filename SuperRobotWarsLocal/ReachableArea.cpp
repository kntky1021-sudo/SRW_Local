#include "ReachableArea.h"
#include "TileMap.h"
#include <queue>
#include <unordered_set>
#include <algorithm>

namespace {
    // 座標をハッシュ化するための構造体
    struct PairHash {
        std::size_t operator()(const std::pair<int, int>& p) const {
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };

    // 地形コストを取得（タイルIDに応じて）
    int getTerrainCost(int tileId) {
        // タイルIDごとの移動コスト定義
        // 0: 壁・障害物（通行不可）
        // 1: 平地
        // 2: 森
        // 3: 山
        // 4: 水
        switch (tileId) {
        case 0: return -1;  // 通行不可
        case 1: return 1;   // 平地
        case 2: return 2;   // 森
        case 3: return 3;   // 山
        case 4: return 2;   // 水
        default: return 1;  // デフォルトは平地扱い
        }
    }
}

std::vector<std::pair<int, int>>
computeReachable(TileMap* map,
    int startX,
    int startY,
    int maxRange)
{
    if (!map) {
        return { {startX, startY} };
    }

    const int mapW = map->getMapWidth();
    const int mapH = map->getMapHeight();

    // 範囲外チェック
    if (startX < 0 || startX >= mapW || startY < 0 || startY >= mapH) {
        return {};
    }

    // BFSで移動可能範囲を探索
    // first: 座標, second: 残り移動力
    std::queue<std::tuple<int, int, int>> q;
    std::unordered_set<std::pair<int, int>, PairHash> visited;
    std::vector<std::pair<int, int>> result;

    q.push({ startX, startY, maxRange });
    visited.insert({ startX, startY });
    result.push_back({ startX, startY });

    // 4方向（上下左右）
    const int dx[] = { 0, 0, -1, 1 };
    const int dy[] = { -1, 1, 0, 0 };

    while (!q.empty()) {
        auto [x, y, remainMove] = q.front();
        q.pop();

        // 4方向を探索
        for (int dir = 0; dir < 4; ++dir) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            // マップ範囲チェック
            if (nx < 0 || nx >= mapW || ny < 0 || ny >= mapH) {
                continue;
            }

            // 既に訪問済み
            if (visited.count({ nx, ny })) {
                continue;
            }

            // 地形コスト取得（TileMapから取得する場合は実装が必要）
            // 現状はタイルIDを直接参照できないため、仮に平地として扱う
            // TODO: TileMap::getTile(x, y) を実装して地形コストを正確に取得
            int cost = 1;  // 仮実装：すべて平地扱い

            // 移動力が足りない
            if (remainMove < cost) {
                continue;
            }

            // 訪問済みマークと結果に追加
            visited.insert({ nx, ny });
            result.push_back({ nx, ny });

            // 次の探索をキューに追加
            q.push({ nx, ny, remainMove - cost });
        }
    }

    return result;
}

// 攻撃可能範囲を計算（移動後の位置から攻撃範囲内）
std::vector<std::pair<int, int>>
computeAttackRange(TileMap* map,
    int startX,
    int startY,
    int moveRange,
    int attackRange)
{
    if (!map) {
        return {};
    }

    // まず移動可能範囲を取得
    auto movableTiles = computeReachable(map, startX, startY, moveRange);

    std::unordered_set<std::pair<int, int>, PairHash> attackTiles;

    // 各移動可能位置から攻撃範囲を計算
    for (const auto& [mx, my] : movableTiles) {
        // 攻撃範囲内のタイルを全て列挙（菱形範囲）
        for (int dy = -attackRange; dy <= attackRange; ++dy) {
            for (int dx = -attackRange; dx <= attackRange; ++dx) {
                // マンハッタン距離で範囲判定
                if (std::abs(dx) + std::abs(dy) <= attackRange) {
                    int ax = mx + dx;
                    int ay = my + dy;

                    // マップ範囲内チェック
                    if (ax >= 0 && ax < map->getMapWidth() &&
                        ay >= 0 && ay < map->getMapHeight()) {
                        attackTiles.insert({ ax, ay });
                    }
                }
            }
        }
    }

    // セットをベクトルに変換
    std::vector<std::pair<int, int>> result(attackTiles.begin(), attackTiles.end());
    return result;
}