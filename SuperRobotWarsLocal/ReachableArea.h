#pragma once

#include <vector>
#include <utility>

class TileMap;

/// @brief now takes four args
std::vector<std::pair<int, int>>
computeReachable(TileMap* map,
    int startX,
    int startY,
    int maxRange);