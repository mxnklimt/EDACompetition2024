#include "readfile.h"
#include "ffdot.h"
#include "KMeans.h"
#include "vector"
#include "isOverlap.h"
#include "ThreadPool.h"
#include "findNonOverlappingPosition.h"
#include <cmath>
#include <utility>
#include <iostream>

//#include <thread>
//#include <queue>
//#include <mutex>
//#include <condition_variable>
//#include <functional>
//#include <future>

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
