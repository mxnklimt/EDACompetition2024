#pragma once
#include <vector>
#include <iostream>
#include "ffdot.h"
// 检查两个点是否重叠
bool isOverlapping(const Point& a, const Point& b) {
    // 检查两个点的矩形区域是否重叠
    bool xOverlap = std::abs(a.x - b.x) < (a.width + b.width) / 2.0;
    bool yOverlap = std::abs(a.y - b.y) < (a.height + b.height) / 2.0;
    return xOverlap && yOverlap;
}

void areClustersOverlapping(const std::vector<std::vector<Point>>& clusters1, const std::vector<std::vector<Point>>& clusters2) {
    // 遍历 clusters1 和 clusters2 中的每个点，并检查是否有重叠
    for (const auto& cluster1 : clusters1) {
        for (const auto& cluster2 : clusters2) {
            for (const auto& point1 : cluster1) {
                for (const auto& point2 : cluster2) {
                    if (isOverlapping(point1, point2)) {
                        std::cout << "Points overlap: Point in cluster1 (" << point1.x << ", " << point1.y << "), Width: "
                            << point1.width << ", Height: " << point1.height << " with Point in cluster2 ("
                            << point2.x << ", " << point2.y << "), Width: " << point2.width << ", Height: "
                            << point2.height << std::endl;
                        return; // 找到重叠点后，直接返回
                    }
                }
            }
        }
    }
    std::cout << "No overlapping points between the clusters." << std::endl; // 没有重叠点
}

