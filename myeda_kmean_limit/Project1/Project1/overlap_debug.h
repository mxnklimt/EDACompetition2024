#pragma once
#include <vector>
#include <iostream>
#include "ffdot.h"
// 检查两个点是否重叠
bool isOverlapping(const Point& p1, const Point& p2) {
    // 计算 p1 的边界
    double left1 = p1.x - p1.width / 2.0;
    double right1 = p1.x + p1.width / 2.0;
    double top1 = p1.y - p1.height / 2.0;
    double bottom1 = p1.y + p1.height / 2.0;

    // 计算 p2 的边界
    double left2 = p2.x - p2.width / 2.0;
    double right2 = p2.x + p2.width / 2.0;
    double top2 = p2.y - p2.height / 2.0;
    double bottom2 = p2.y + p2.height / 2.0;

    // 判断重叠
    bool overlapX = left1 < right2&& left2 < right1;
    bool overlapY = top1 < bottom2&& top2 < bottom1;

    return overlapX && overlapY; // 返回两个方向都重叠的结果
}

// 判断 clusters_buff 中的所有点是否存在重叠，并打印出重叠的点对
void checkOverlapInClusters(const std::vector<std::vector<Point>>& clusters_buff) {
    for (size_t k = 0; k < clusters_buff.size(); ++k) {
        const auto& cluster = clusters_buff[k];
        int n = cluster.size();
        // 对每个 cluster 中的点进行两两比较
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (isOverlapping(cluster[i], cluster[j])) {
                    std::cout << "Cluster " << k << " 中的点 ("
                        << "x1: " << cluster[i].x << ", y1: " << cluster[i].y
                        << ") 和 (x2: " << cluster[j].x << ", y2: " << cluster[j].y
                        << ") 存在重叠。" << std::endl;
                }
                else
                {
                    cout << "/" << endl;
                }
                
            }
        }
    }
}

