#pragma once
#include <vector>
#include <utility>
#include <future>
#include "ThreadPool.h" 
#include "ffdot.h" 
#include "isOverlap.h" 
#include "Manhattan_distance.h"
std::pair<int, int> findNonOverlappingPosition(int center_x, int center_y, int buff_width, int buff_height, 
    const std::vector<std::vector<Point>>& clusters, int x_max, int x_min, int y_max, int y_min, 
    std::vector<std::vector<Point>>& clusters_buff1, std::vector<std::vector<Point>>& clusters_buff2,
    std::vector<double> &clusters_buff_MHTdistance,int fori,double net_unit_c,double net_unit_r,double max_net_rc,int number,
    const std::vector<std::vector<Point>>& clusters_buff) {
    //buff_flag==1,则只需要判断clusters_buff1和clusters是否与新增buff重叠
    //buff_flag==2,则需要判断clusters_buff2和clusters_buff1和clusters是否与新增buff重叠
    const int initial_step = 100;//初始化步长
    int max_radius = 300;//初始最大化半径
    const double radius_multiplier = 2;//半径倍乘因子
    ThreadPool pool(7);//创建一个7线程线程池
    std::vector<std::future<std::pair<int, int>>> results;//定义一个储存std::future<std::pair<int, int>> 的向量，用于存储异步任务的结果

    while (true) {
        for (int radius = 100; radius <= max_radius; radius = radius * 2) {  //从radius半径100开始，每次*2
            int step = initial_step + radius / 10;//计算初始步长
            for (int dx = -radius; dx <= radius; dx += step) {//两层for循环遍历边长为radius的正方形，搜索步长精度step
                for (int dy = -radius; dy <= radius; dy += step) {
                    results.emplace_back(pool.enqueue([=, &clusters, &clusters_buff_MHTdistance] {//将一个异步任务添加到线程池中，pool.enqueue:
                    //[=, &clusters]:这是一个捕获列表，用于指定 lambda 函数如何捕获外部变量。[=] 表示按值捕获所有外部变量。&clusters 表示按引用捕获 clusters 变量。这是因为 clusters 可能是一个较大的数据结构，按引用捕获可以避免不必要的拷贝。
                    //该 lambda 函数捕获所有外部变量（包括 center_x, center_y, buff_width, buff_height, initial_step, max_radius, radius_multiplier, dx, dy 等）按值捕获，并按引用捕获 clusters。
                    //这个 lambda 函数将在一个独立的线程中执行。    
                        int new_x = center_x + dx;//新建buff的坐标
                        int new_y = center_y + dy;

                        if ((!isOverlap(new_x, new_y, buff_width, buff_height, clusters)) &&
                            (!isOverlap(new_x, new_y, buff_width, buff_height, clusters_buff1)) &&
                            (new_x > x_min) && (new_x < x_max) && (new_y > y_min) && (new_y < y_max) &&
                            ((!isOverlap(new_x, new_y, buff_width, buff_height, clusters_buff2)))) {
                            //检查新建buff是否与现有的点碰撞重叠,检查buff是否与已有buff重叠，检查buff是否安放区域不在area内
                            if(number==1)
                            {
                                for (const auto& point : clusters[fori])
                                {
                                    clusters_buff_MHTdistance[fori] = clusters_buff_MHTdistance[fori] + Manhattan_distance(point, new_x, new_y) * Manhattan_distance(point, new_x, new_y);
                                }
                                clusters_buff_MHTdistance[fori] = 0.5 * net_unit_c * net_unit_r * clusters_buff_MHTdistance[fori];
                                if (clusters_buff_MHTdistance[fori] < max_net_rc)
                                {
                                    return std::make_pair(new_x, new_y);//如果不重叠，返回新建buff的坐标
                                }
                            }
                            else if (number == 2)
                            {
                                for (const auto& point : clusters_buff[fori])
                                {
                                    clusters_buff_MHTdistance[fori] = clusters_buff_MHTdistance[fori] + Manhattan_distance(point, new_x, new_y) * Manhattan_distance(point, new_x, new_y);
                                }
                                clusters_buff_MHTdistance[fori] = 0.5 * net_unit_c * net_unit_r * clusters_buff_MHTdistance[fori];
                                if (clusters_buff_MHTdistance[fori] < max_net_rc)
                                {
                                    return std::make_pair(new_x, new_y);//如果不重叠，返回新建buff的坐标
                                }
                            }
                            
                            
                        }
                        return std::make_pair(-1, -1);//如果重叠，返回（-1，-1）
                        }));
                }
            }
        }

        for (auto&& result : results) {
            //使用 for (auto&& result : results) 遍历集合 results。
            //auto 是自动类型推导的关键字，编译器会根据表达式的类型自动推导出 result 的实际类型。
            //result : results：result 是 results 集合中的每个元素。
            auto pos = result.get();// 获取异步任务的结果
            //auto根据result.get()自动推导出pos的类型
            //pos 的类型是 std::pair<int, int>，表示找到的非重叠位置的坐标
            //这句代码会阻塞当前线程，直到异步任务完成，并返回任务的结果。
            if (pos.first != -1 && pos.second != -1) {// 检查结果是否有效
                return pos;//如果找到非重叠位置，返回该位置
            }
        }
        results.clear();
        max_radius = static_cast<int>(max_radius * radius_multiplier);//如果代码运行到这里，说明没有找到非重叠位置，将最大半径扩大
    }
}