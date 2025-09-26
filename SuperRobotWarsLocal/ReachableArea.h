#pragma once

#include <vector>
#include <utility>

class TileMap;

/// @brief マップ上の到達可能範囲を計算する (未実装 stub)
std::vector<std::pair<int, int>>
computeReachable(TileMap* map,
    int startX,
    int startY,
    int maxRange);