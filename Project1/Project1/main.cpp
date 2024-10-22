#include "readfile.h"
#include "ffdot.h"
#include "KMeans.h"
#include "vector"
#include <cmath>
#include <utility>
#include <iostream>

// 定义一个新的结构体 Point，包含坐标和尺寸
struct Point {
    int x;
    int y;
    int width;
    int height;
};

// 修改 isOverlap 函数，考虑每个点的尺寸
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

// 修改 findNonOverlappingPosition 函数，使用新的 Point 结构体
std::pair<int, int> findNonOverlappingPosition(int center_x, int center_y, int buff_width, int buff_height, const std::vector<std::vector<Point>>& clusters) {
    const int initial_step = 1; // 初始步长
    int max_radius = 100; // 初始最大搜索半径
    const double radius_multiplier = 1.5; // 指数增长因子

    while (true) {
        for (int radius = 1; radius <= max_radius; ++radius) {
            int step = initial_step + radius / 10; // 随着半径增加步长
            for (int dx = -radius; dx <= radius; dx += step) {
                for (int dy = -radius; dy <= radius; dy += step) {
                    int new_x = center_x + dx;
                    int new_y = center_y + dy;
                    if (!isOverlap(new_x, new_y, buff_width, buff_height, clusters)) {
                        return {new_x, new_y};
                    }
                }
            }
        }
        max_radius = static_cast<int>(max_radius * radius_multiplier); // 指数方式扩大搜索范围
    }
}

int main()
{
    readedafile myfile;

    myfile.setfilename("constraints.txt", "problem.def");
    myfile.get_constraintstxt();
    myfile.get_ffdot();

    const int size = static_cast<int>(myfile.myffdot.numofdot); //Number of samples
    const int dim = 2;   //Dimension of feature
    const int cluster_num = 1876; //Cluster number
    KMeans* kmeans = new KMeans(dim, cluster_num);
    int* labels = new int[size];
    kmeans->SetInitMode(KMeans::InitUniform);
    kmeans->Cluster(myfile.myffdot.data, size, labels); 

    // 创建一个二维数组来存储每个簇的点
    std::vector<std::vector<Point>> clusters(cluster_num);

    for (int i = 0; i < size; ++i)
    {
        int cluster_id = labels[i];
        int x = static_cast<int>(myfile.myffdot.data[i * dim + 0]);
        int y = static_cast<int>(myfile.myffdot.data[i * dim + 1]);
        int width = static_cast<int>(myfile.myffdot.my_ffsize.x);
        int height = static_cast<int>(myfile.myffdot.my_ffsize.y);
        clusters[cluster_id].emplace_back(Point{x, y, width, height});
    }

    for (int i = 0; i < cluster_num; ++i)
    {
        printf("Cluster %d:\n", i);
        for (const auto& point : clusters[i])
        {
            printf("%d, %d, %d, %d\n", point.x, point.y, point.width, point.height);
        }
    }

    // 计算每个簇的中心位置并放置缓冲区
    for (int i = 0; i < cluster_num; ++i)
    {
        int sum_x = 0, sum_y = 0;
        for (const auto& point : clusters[i])
        {
            sum_x += point.x;
            sum_y += point.y;
        }
        int center_x = sum_x / clusters[i].size(); // 簇的中心x坐标
        int center_y = sum_y / clusters[i].size(); // 簇的中心y坐标

        int buff_width = static_cast<int>(myfile.myffdot.my_buffsize.x);
        int buff_height = static_cast<int>(myfile.myffdot.my_buffsize.y);

        // 检查缓冲区是否与所有簇中的任何点重叠
        if (!isOverlap(center_x, center_y, buff_width, buff_height, clusters)) {
            printf("Buffer placed at cluster %d center: (%d, %d)\n", i, center_x, center_y);
        } else {
            // 在中心位置附近寻找合适的缓冲区位置
            auto [new_x, new_y] = findNonOverlappingPosition(center_x, center_y, buff_width, buff_height, clusters);
            if (new_x == center_x && new_y == center_y) {
                std::cout << "error" << std::endl;
            } else {
                printf("Buffer placed at cluster %d near center: (%d, %d)\n", i, new_x, new_y);
            }
        }
    }

    delete[] labels;
    delete kmeans;
    delete myfile.myffdot.data;
    return 0;
}
