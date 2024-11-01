#pragma once
#include <vector>
#include <iostream>
#include "ffdot.h"
// ����������Ƿ��ص�
bool isOverlapping(const Point& p1, const Point& p2) {
    // ���� p1 �ı߽�
    double left1 = p1.x - p1.width / 2.0;
    double right1 = p1.x + p1.width / 2.0;
    double top1 = p1.y - p1.height / 2.0;
    double bottom1 = p1.y + p1.height / 2.0;

    // ���� p2 �ı߽�
    double left2 = p2.x - p2.width / 2.0;
    double right2 = p2.x + p2.width / 2.0;
    double top2 = p2.y - p2.height / 2.0;
    double bottom2 = p2.y + p2.height / 2.0;

    // �ж��ص�
    bool overlapX = left1 < right2&& left2 < right1;
    bool overlapY = top1 < bottom2&& top2 < bottom1;

    return overlapX && overlapY; // �������������ص��Ľ��
}

// �ж� clusters_buff �е����е��Ƿ�����ص�������ӡ���ص��ĵ��
void checkOverlapInClusters(const std::vector<std::vector<Point>>& clusters_buff) {
    for (size_t k = 0; k < clusters_buff.size(); ++k) {
        const auto& cluster = clusters_buff[k];
        int n = cluster.size();
        // ��ÿ�� cluster �еĵ���������Ƚ�
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (isOverlapping(cluster[i], cluster[j])) {
                    std::cout << "Cluster " << k << " �еĵ� ("
                        << "x1: " << cluster[i].x << ", y1: " << cluster[i].y
                        << ") �� (x2: " << cluster[j].x << ", y2: " << cluster[j].y
                        << ") �����ص���" << std::endl;
                }
                else
                {
                    cout << "/" << endl;
                }
                
            }
        }
    }
}

