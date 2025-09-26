#pragma once
#pragma once

#include <vector>
#include <utility>

class TileMap;

/// @brief スタート地点 (startX,startY) から moveRange 以内に到達可能なマスを計算する
/// @return 到達可能な (x,y) 座標のペア集合
std::vector<std::pair<int, int>> computeReachable(
    TileMap* tileMap,
    int startX,
    int startY,
    int moveRange);