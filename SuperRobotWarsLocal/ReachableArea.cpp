#include "ReachableArea.h"
#include "TileMap.h"
#include <queue>
#include <limits>

struct Node {
    int x, y;
    int cost;
};

struct Compare {
    bool operator()(Node const& a, Node const& b) const {
        return a.cost > b.cost;
    }
};

std::vector<std::pair<int, int>> computeReachable(
    TileMap* tileMap,
    int startX,
    int startY,
    int moveRange)
{
    int w = tileMap->getMapWidth();
    int h = tileMap->getMapHeight();

    // コスト配列 (幅×高)
    std::vector<int> dist(w * h, std::numeric_limits<int>::max());
    std::priority_queue<Node, std::vector<Node>, Compare> pq;

    // 開始位置を初期化
    if (startX < 0 || startY < 0 || startX >= w || startY >= h) {
        return {};
    }
    int si = startY * w + startX;
    dist[si] = 0;
    pq.push({ startX, startY, 0 });

    // 4方向
    const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };

    while (!pq.empty()) {
        auto cur = pq.top(); pq.pop();
        int ci = cur.y * w + cur.x;
        if (cur.cost > dist[ci]) continue;

        for (auto& d : dirs) {
            int nx = cur.x + d[0];
            int ny = cur.y + d[1];
            if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;

            int tileCost = tileMap->getCost(nx, ny);
            int newCost = cur.cost + tileCost;
            int ni = ny * w + nx;

            if (newCost <= moveRange && newCost < dist[ni]) {
                dist[ni] = newCost;
                pq.push({ nx, ny, newCost });
            }
        }
    }

    // moveRange以内を結果に格納
    std::vector<std::pair<int, int>> result;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (dist[y * w + x] <= moveRange) {
                result.emplace_back(x, y);
            }
        }
    }
    return result;
}