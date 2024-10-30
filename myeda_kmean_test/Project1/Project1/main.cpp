#include "readfile.h"
#include "ffdot.h"
#include "isOverlap.h"
#include "ThreadPool.h"
#include "findNonOverlappingPosition.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>



double distance(const Point& p1, const Point& p2) {
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

std::vector<Point> kmeans(std::vector<Point>& points, int k, int max_iterations,int numofcomp,int *labels,int max_cluster_point) {
    std::vector<Point> centroids(k);
    std::vector<int> cluster_sizes(k, 0);
    std::vector<std::vector<Point>> clusters(k);

    cout<<"im ok 1"<<endl;

    // Initialize centroids randomly
    for (int i = 0; i < k; ++i) {
        centroids[i] = points[rand() % (numofcomp-1)];
    }
    cout<<"im ok 2"<<endl;
    for (int iter = 0; iter < max_iterations; ++iter) 
    {
        // Clear clusters
        for (int i = 0; i < k; ++i) {
            clusters[i].clear();
            cluster_sizes[i] = 0;
        }

        // Assign points to the nearest centroid
        for(int j=0;j<numofcomp;j++)
        {
           
            double best_distance = std::numeric_limits<double>::max();
            int best_cluster = 0;
            for (int i = 0; i < k; ++i) 
            {
                // max_cluster_point = myfile.max_fanout - 1每个聚类中最多元素数量，即每个buff连接的数量
                
                if (cluster_sizes[i] < max_cluster_point)
                {
                    double dist = distance(points[j], centroids[i]);
                    if (dist < best_distance) {
                        best_distance = dist;
                        best_cluster = i;
                    }
                 
                } 
            } 
            clusters[best_cluster].push_back(points[j]);
            cluster_sizes[best_cluster]++;              
            labels[j] = best_cluster;
        }


        // Update centroids
        bool centroids_changed = false;
        for (int i = 0; i < k; ++i) {
            Point new_centroid = {0, 0};
            for (const auto& point : clusters[i]) {
                new_centroid.x += point.x;
                new_centroid.y += point.y;
            }
            if (!clusters[i].empty()) {
                new_centroid.x /= clusters[i].size();
                new_centroid.y /= clusters[i].size();
            }
            if (distance(new_centroid, centroids[i]) > 1e-9) {
                centroids_changed = true;
            }
            centroids[i] = new_centroid;
        }

        // If centroids didn't change, stop iterating
        if (!centroids_changed) {
            break;
        }
        
    }
    
    int total_com=0;
    for(int i=0;i<k;i++)
    {
        cout<<cluster_sizes[i]<<endl;
        total_com+=cluster_sizes[i];
    }
    cout<<total_com<<endl;

    return centroids;
}


int main()
{
    srand(time(0));
    readedafile myfile;

    myfile.setfilename("constraints.txt", "problem.def");
    myfile.get_constraintstxt();
    myfile.get_ffdot();
    cout << "ffdot.area.area1.x" << myfile.myffdot.area.area1.x << endl;
    cout << "ffdot.area.area1.y" << myfile.myffdot.area.area1.y << endl;
    cout << "ffdot.area.area2.x" << myfile.myffdot.area.area2.x << endl;
    cout << "ffdot.area.area2.y" << myfile.myffdot.area.area2.y << endl;
    cout << "ffdot.area.area3.x" << myfile.myffdot.area.area3.x << endl;
    cout << "ffdot.area.area3.y" << myfile.myffdot.area.area3.y << endl;
    cout << "ffdot.area.area4.x" << myfile.myffdot.area.area4.x << endl;
    cout << "ffdot.area.area4.y" << myfile.myffdot.area.area4.y << endl;
    //生成的buff要在上述四个点构成的长方形内
    std::vector<int> ffdot_area_x = {myfile.myffdot.area.area1.x, myfile.myffdot.area.area2.x, myfile.myffdot.area.area3.x, myfile.myffdot.area.area4.x};
    int ffdot_area_x_min = *std::min_element(ffdot_area_x.begin(), ffdot_area_x.end());//x_min
    int ffdot_area_x_max = *std::max_element(ffdot_area_x.begin(), ffdot_area_x.end());//x_max
    std::vector<int> ffdot_area_y = {myfile.myffdot.area.area1.y, myfile.myffdot.area.area2.y, myfile.myffdot.area.area3.y, myfile.myffdot.area.area4.y};
    int ffdot_area_y_min = *std::min_element(ffdot_area_y.begin(), ffdot_area_y.end());//y_min
    int ffdot_area_y_max = *std::max_element(ffdot_area_y.begin(), ffdot_area_y.end());//y_max
    cout << "ffdot.area.x_min" << ffdot_area_x_min << endl;
    cout << "ffdot.area.x_max" << ffdot_area_x_max << endl;
    cout << "ffdot.area.y_min" << ffdot_area_y_min << endl;
    cout << "ffdot.area.y_max" << ffdot_area_y_max << endl;


    const int size = static_cast<int>(myfile.myffdot.numofdot); //Number of Point
    const int dim = 2;   //Dimension of feature
    const int cluster_num = ceil((float)size/(myfile.max_fanout-1)); //Cluster number
    // const int cluster_num =100; //Cluster number

    int* labels = new int[size];

    cout << size << endl;
    cout << cluster_num << endl;

    vector<int> buff_cluster_num;
    int count_cluster=cluster_num;
    while(count_cluster>myfile.max_fanout-1)//循环计算要多少个buff聚类
    {
        count_cluster=ceil((float)count_cluster/(myfile.max_fanout-1));
        buff_cluster_num.emplace_back(count_cluster);
        // cout << "count_cluster" << endl;
        // cout << count_cluster << endl;
    }
    buff_cluster_num.emplace_back(1);//最后一个buff连接之前buff
    
    for(int i = 0; i < buff_cluster_num.size(); ++i) {
        std::cout << buff_cluster_num[i] << endl;
    }
    std::cout <<" ---------------------------------------1" << endl;
    int total_cluster_time = buff_cluster_num.size();//计算要创建多个buff_buff_info

    std::vector<Point> points(myfile.myffdot.numofdot);
    for (size_t i = 0; i < myfile.myffdot.numofdot; i++) {
        points[i].x=myfile.myffdot.data[i*2];
        points[i].y=myfile.myffdot.data[i*2+1];
    }    



    int k = cluster_num; // Number of clusters
    int max_iterations = 5;
    std::cout << myfile.myffdot.numofdot << endl;
    std::vector<Point> centroids = kmeans(points, k, max_iterations,myfile.myffdot.numofdot,labels, myfile.max_fanout - 1);//// max_cluster_point = myfile.max_fanout - 1每个聚类中最多元素数量，即每个buff连接的数量

    // std::cout << "Centroids:" << std::endl;
    // for (const auto& centroid : centroids) {
    //     std::cout << "(" << centroid.x << ", " << centroid.y << ")" << std::endl;
    // }

    // 创建一个二维数组来存储每个簇的点
    std::vector<std::vector<Point>> clusters(cluster_num);

    for (int i = 0; i < size; ++i)
    {
        int cluster_id = labels[i];
        int x = static_cast<int>(myfile.myffdot.data[i * dim + 0]);
        int y = static_cast<int>(myfile.myffdot.data[i * dim + 1]);
        int width = static_cast<int>(myfile.myffdot.my_ffsize.x);
        int height = static_cast<int>(myfile.myffdot.my_ffsize.y);
        clusters[cluster_id].emplace_back(Point{ x, y, width, height });
    }//Put the data into std::vector<std::vector<Point>> clusters(cluster_num);

    for (int i = 0; i < cluster_num; ++i)
    {
        printf("Cluster %d:\n", i);
        int count = 0;
        for (const auto& point : clusters[i])
        {
            printf("%d, %d, %d, %d\n", point.x, point.y, point.width, point.height);
            count++;
        }
        cout << count << endl;
    }//print cluster and position

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
            printf("buffer placed at cluster %d center: (%d, %d)\n", i, center_x, center_y);
        }
        else {
            // 在中心位置附近寻找合适的缓冲区位置
            auto [new_x, new_y] = findNonOverlappingPosition(center_x, center_y, buff_width, buff_height, clusters, ffdot_area_x_max, ffdot_area_x_min, ffdot_area_y_max, ffdot_area_y_min);
             //ThreadPool!!!!! 
            if (new_x == center_x && new_y == center_y) {
                std::cout << "error" << std::endl;
            }
            else {
                printf("buffer placed at cluster %d near center: (%d, %d)\n", i, new_x, new_y);
                // 将新找到的非重叠位置添加到 clusters 中，从而避免其他缓冲区放置在这个位置
                clusters.push_back({ Point{new_x, new_y, buff_width, buff_height} });
            }
        }
    }

    //10/29 V4.3
    const int size_2 = cluster_num; //Number of Point
    const int cluster_num_2 = 50; //Cluster number
    int* labels_2 = new int[size_2];
    vector<int> buff_cluster_num_2;
    int count_cluster_2 = cluster_num_2;

    delete[] labels;

    delete myfile.myffdot.data;
    return 0;
}
