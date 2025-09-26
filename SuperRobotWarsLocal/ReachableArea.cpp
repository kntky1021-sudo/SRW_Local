#include "ReachableArea.h"
#include "TileMap.h"

std::vector<std::pair<int, int>>
computeReachable(TileMap* map,
    int startX,
    int startY,
    int maxRange)
{
    // TODO: 実際の移動コスト計算＆ BFS/A* に置き換え
    // stub: 開始点のみ返却
    (void)map; (void)maxRange;
    return { { startX, startY } };
}