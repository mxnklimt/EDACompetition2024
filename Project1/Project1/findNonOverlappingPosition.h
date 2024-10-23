#pragma once
#include <vector>
#include <utility>
#include <future>
#include "ThreadPool.h" // 假设您有一个自定义的 ThreadPool 类
#include "ffdot.h" // 假设您有一个自定义的 Point 类
#include "isOverlap.h" // 假设您有一个自定义的 isOverlap 函数
std::pair<int, int> findNonOverlappingPosition(int center_x, int center_y, int buff_width, int buff_height, const std::vector<std::vector<Point>>& clusters) {
    const int initial_step = 1;
    int max_radius = 100;
    const double radius_multiplier = 1.5;
    ThreadPool pool(8);
    std::vector<std::future<std::pair<int, int>>> results;

    while (true) {
        for (int radius = 1; radius <= max_radius; ++radius) {
            int step = initial_step + radius / 10;
            for (int dx = -radius; dx <= radius; dx += step) {
                for (int dy = -radius; dy <= radius; dy += step) {
                    results.emplace_back(pool.enqueue([=, &clusters] {
                        int new_x = center_x + dx;
                        int new_y = center_y + dy;
                        if (!isOverlap(new_x, new_y, buff_width, buff_height, clusters)) {
                            return std::make_pair(new_x, new_y);
                        }
                        return std::make_pair(-1, -1);
                        }));
                }
            }
        }

        for (auto&& result : results) {
            auto pos = result.get();
            if (pos.first != -1 && pos.second != -1) {
                return pos;
            }
        }
        results.clear();
        max_radius = static_cast<int>(max_radius * radius_multiplier);
    }
}