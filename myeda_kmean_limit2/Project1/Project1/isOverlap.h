
#pragma once
#include <vector>
#include <cmath>
#include "ffdot.h"
// 修改 isOverlap 函数，考虑每个点的尺寸
// 依赖：Point结构体
// 修改 isOverlap 函数，使每个坐标作为左下角坐标
bool isOverlap(int x, int y, int buff_width, int buff_height, const std::vector<std::vector<Point>>& clusters) {
    for (const auto& points : clusters) {
        for (const auto& point : points) {
            // 检查缓冲矩形和 point 矩形是否重叠
            if (x < point.x + point.width && x + buff_width > point.x &&
                y < point.y + point.height && y + buff_height > point.y) {
                return true;
            }
        }
    }
    return false;
}


