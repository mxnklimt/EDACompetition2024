
#pragma once
#include <vector>
#include <cmath>
#include "ffdot.h"
// �޸� isOverlap ����������ÿ����ĳߴ�
// ������Point�ṹ��
// �޸� isOverlap ������ʹÿ��������Ϊ���½�����
bool isOverlap(int x, int y, int buff_width, int buff_height, const std::vector<std::vector<Point>>& clusters) {
    for (const auto& points : clusters) {
        for (const auto& point : points) {
            // ��黺����κ� point �����Ƿ��ص�
            if (x < point.x + point.width && x + buff_width > point.x &&
                y < point.y + point.height && y + buff_height > point.y) {
                return true;
            }
        }
    }
    return false;
}


