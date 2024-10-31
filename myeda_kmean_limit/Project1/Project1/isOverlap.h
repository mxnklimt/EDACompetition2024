
#pragma once
#include <vector>
#include <cmath>
#include "ffdot.h"
// �޸� isOverlap ����������ÿ����ĳߴ�
// ������Point�ṹ��
bool isOverlap(int x, int y, int buff_width, int buff_height, const std::vector<std::vector<Point>>& clusters) {
    for (const auto& points : clusters) {
        for (const auto& point : points) {
            if (std::abs(point.x - x) < (buff_width + point.width) / 2 && std::abs(point.y - y) < (buff_height + point.height) / 2) {
                return true;
            }
        }
    }
    return false;
}

