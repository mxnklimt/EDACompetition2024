#include "readfile.h"
#include "writefile.h"
#include "ffdot.h"
#include "isOverlap.h"
#include "ThreadPool.h"
#include "findNonOverlappingPosition.h"
#include "overlap_debug.h"
#include "Manhattan_distance.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>







double distance(const Point& p1, const Point& p2) {
    //return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    return (p1.x - p2.x) + (p1.y - p2.y);
}

std::vector<Point> kmeans(std::vector<Point>& points, int k, int max_iterations,int numofcomp,int *labels,int max_cluster_point) {
    std::vector<Point> centroids(k);
    std::vector<int> cluster_sizes(k, 0);
    std::vector<std::vector<Point>> clusters(k);

    // Initialize centroids randomly
    for (int i = 0; i < k; ++i) {
        centroids[i] = points[rand() % (numofcomp-1)];
    }
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
    
    // int total_com=0;
    // for(int i=0;i<k;i++)
    // {
    //     cout<<cluster_sizes[i]<<endl;
    //     total_com+=cluster_sizes[i];
    // }
    // cout<<total_com<<endl;

    return centroids;
}


int main()
{
    srand(time(0));
    readedafile myfile;

    myfile.setfilename("constraints.txt", "problem.def");
    myfile.get_constraintstxt();
    myfile.get_ffdot();
    
    std::vector<int> ffdot_area_x = { myfile.myffdot.area.area1.x, myfile.myffdot.area.area2.x, myfile.myffdot.area.area3.x, myfile.myffdot.area.area4.x };
    int ffdot_area_x_min = *std::min_element(ffdot_area_x.begin(), ffdot_area_x.end());//x_min
    int ffdot_area_x_max = *std::max_element(ffdot_area_x.begin(), ffdot_area_x.end());//x_max
    std::vector<int> ffdot_area_y = { myfile.myffdot.area.area1.y, myfile.myffdot.area.area2.y, myfile.myffdot.area.area3.y, myfile.myffdot.area.area4.y };
    int ffdot_area_y_min = *std::min_element(ffdot_area_y.begin(), ffdot_area_y.end());//y_min
    int ffdot_area_y_max = *std::max_element(ffdot_area_y.begin(), ffdot_area_y.end());//y_max
    //cout << "ffdot.area.x_min" << ffdot_area_x_min << endl;
    //cout << "ffdot.area.x_max" << ffdot_area_x_max << endl;
    //cout << "ffdot.area.y_min" << ffdot_area_y_min << endl;
    //cout << "ffdot.area.y_max" << ffdot_area_y_max << endl;

    const int size = static_cast<int>(myfile.myffdot.numofdot); //Number of Point
    const int dim = 2;   //Dimension of feature
    const int cluster_num = ceil((float)size/(myfile.max_fanout-1)); //Cluster number
    const int buff_buff_cluster_num = ceil((float)cluster_num/(myfile.max_fanout-1)); //buff Cluster number
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

    int total_cluster_time = buff_cluster_num.size();//计算要创建多个buff_buff_info
    cout<<"------------------ff kmeans begin--------------------"<<endl;
    std::vector<Point> points(myfile.myffdot.numofdot);
    for (size_t i = 0; i < myfile.myffdot.numofdot; i++) {
        points[i].x=myfile.myffdot.data[i*2];
        points[i].y=myfile.myffdot.data[i*2+1];
    }    



    int k = cluster_num; // Number of clusters
    int max_iterations = 5;

    std::vector<Point> centroids = kmeans(points, k, max_iterations,myfile.myffdot.numofdot,labels, myfile.max_fanout - 1);//// max_cluster_point = myfile.max_fanout - 1每个聚类中最多元素数量，即每个buff连接的数量   

    cout<<"------------------ff kmeans ok--------------------"<<endl;
    writeedafile* writefile=new writeedafile(size,cluster_num,total_cluster_time,myfile.max_fanout,&myfile,buff_buff_cluster_num);

    // 创建一个二维数组来存储每个簇的点
    std::vector<std::vector<Point>> clusters(cluster_num);

    for (int i = 0; i < size; ++i)
    {
        int cluster_id = labels[i];
        int x = static_cast<int>(myfile.myffdot.data[i * dim + 0]);
        int y = static_cast<int>(myfile.myffdot.data[i * dim + 1]);
        int width = static_cast<int>(myfile.myffdot.my_ffsize.x);
        int height = static_cast<int>(myfile.myffdot.my_ffsize.y);
        clusters[cluster_id].emplace_back(Point{ (double)x, (double)y, width, height });
    }//Put the data into std::vector<std::vector<Point>> clusters(cluster_num);

    writefile->get_ffinfo(labels,myfile.myffdot.data,size);



    ff_buff_info* info_buff;
    info_buff=new ff_buff_info[cluster_num];

   
    double* buffcoord=new double[cluster_num*2]();//buff的坐标

    // 创建一个二维数组来存储buff1
    std::vector<std::vector<Point>> clusters_buff1(1);


    //创建一个数组来储存buff1的曼哈顿距离平方和,初始化为0
    std::vector<double> clusters_buff1_MHTdistance(cluster_num,0.0);



    // 计算每个簇的中心位置并放置缓冲区
    for (int i = 0; i < cluster_num; ++i)
    {
        info_buff[i].buffname=to_string((int)writefile->buffernamecount);
        writefile->buffernamecount++;
        // info_buff[i].connect_ff_num=0;
        info_buff[i].front_clusternum=i;

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
        // 在中心位置附近寻找合适的缓冲区位置
        auto [new_x, new_y] = findNonOverlappingPosition(center_x, center_y, buff_width, buff_height,
            clusters, ffdot_area_x_max, ffdot_area_x_min, ffdot_area_y_max, ffdot_area_y_min, 
            clusters_buff1);  //这里Cluster_buff1的调用没用
      
             //ThreadPool!!!!! 

            
        //printf("buffer placed at cluster %d near center: (%d, %d)\n", i, new_x, new_y);
        info_buff[i].x=new_x;
        info_buff[i].y=new_y;
        buffcoord[i*2]=new_x;
        buffcoord[i*2+1]=new_y;



		//计算RC step1，算第一部分曼哈顿平方和

		clusters_buff1[0].push_back({ Point{(double)new_x, (double)new_y, buff_width, buff_height} });//非聚类中心buff存入
		//std::cout << clusters_buff1[0][i].x << std::endl;
		//将数据Point类型存在clusters_buff1[0]中，所以clusters_buff1[0]中会有一千多个buff的数据
           
        
    }
    //for (const auto& cluster : clusters_buff1) {  // 遍历每个簇
    //    for (const auto& point : cluster) {       // 遍历簇中的每个点
    //        std::cout << "Point: (" << point.x << ", " << point.y << ")" << std::endl;
    //        //cout << "!isOverlap(new_x, new_y, buff_width, buff_height, clusters) = " << !isOverlap(point.x, point.y, static_cast<int>(myfile.myffdot.my_buffsize.x), static_cast<int>(myfile.myffdot.my_buffsize.y), clusters) << endl;
    //        cout << "!isOverlap(new_x, new_y, buff_width, buff_height, clusters_buff1) = " << !isOverlap(point.x, point.y, static_cast<int>(myfile.myffdot.my_buffsize.x), static_cast<int>(myfile.myffdot.my_buffsize.y), clusters_buff1) << endl;
    //    }
    //}
     cout << "---------------------------- - debug1--------------------" << endl;
    // areClustersOverlapping(clusters_buff1, clusters); // 调用函数并打印结果
    cout << "---------------------------- - debug1--------------------" << endl;
    writefile->get_ff_buff_info(info_buff,cluster_num,size);
    

    
    int* bufflabels  = new int[cluster_num]();//buff聚类的labels
    cout<<buff_buff_cluster_num<<endl;
    cout<<"------------------buff kmeans begin--------------------"<<endl;
    std::vector<Point> points_buff(cluster_num);
    for (size_t i = 0; i < cluster_num; i++) {
        points_buff[i].x=buffcoord[i*2];
        points_buff[i].y=buffcoord[i*2+1];
    }    

    int k1 = buff_buff_cluster_num; // Number of clusters
    int max_iterations_buff = 100;

    std::vector<Point> centroids1 = kmeans(points_buff, k1, max_iterations_buff,cluster_num,bufflabels, myfile.max_fanout - 1);//// max_cluster_point = myfile.max_fanout - 1每个聚类中最多元素数量，即每个buff连接的数量   
    cout<<"------------------buff kmeans ok--------------------"<<endl;

    // 创建一个二维数组来存储每个簇的点
    std::vector<std::vector<Point>> clusters_buff(buff_buff_cluster_num);



    for (int i = 0; i < cluster_num; ++i)
    {
        int cluster_id = bufflabels[i];
        //bufflabels里面存了buff1对应的buff2编号
        int x = static_cast<int>(buffcoord[i * dim + 0]);
        int y = static_cast<int>(buffcoord[i * dim + 1]);
        int width = static_cast<int>(myfile.myffdot.my_buffsize.x);
        int height = static_cast<int>(myfile.myffdot.my_buffsize.y);
        clusters_buff[cluster_id].emplace_back(Point{ (double)x, (double)y, width, height });
        
        //强制转换为double
    }//Put the data into std::vector<std::vector<Point>> clusters(cluster_num);




    writefile->get_buffinfo(bufflabels,buffcoord,cluster_num,info_buff);





    buff_buff_info* info_buff_buff;
    info_buff_buff=new buff_buff_info[buff_buff_cluster_num]();
    // 计算每个簇的中心位置并放置缓冲区
    for (int i = 0; i < buff_buff_cluster_num; ++i)
    {
        info_buff_buff[i].buffname=to_string((int)writefile->buffernamecount);
        writefile->buffernamecount++;
        // info_buff[i].connect_ff_num=0;
        info_buff_buff[i].front_clusternum=i;

        int sum_x = 0, sum_y = 0;
        for (const auto& point : clusters_buff[i])
        {
            sum_x += point.x;
            sum_y += point.y;
        }
        int center_x = sum_x / clusters_buff[i].size(); // 簇的中心x坐标
        int center_y = sum_y / clusters_buff[i].size(); // 簇的中心y坐标

        //cluster_buff[i]可以获得当前buff2连接的所有buff1的数据
        int buff_width = static_cast<int>(myfile.myffdot.my_buffsize.x);
        int buff_height = static_cast<int>(myfile.myffdot.my_buffsize.y);



        
		//printf("buffer2 placed at cluster %d center: (%d, %d)\n", i, center_x, center_y);
		// 在中心位置附近寻找合适的缓冲区位置
		auto [new_x, new_y] = findNonOverlappingPosition(center_x, center_y, buff_width, buff_height, 
            clusters, ffdot_area_x_max, ffdot_area_x_min, ffdot_area_y_max, ffdot_area_y_min, 
            clusters_buff1);
		//ThreadPool!!!!! 



		   //printf("buffer2 placed at cluster %d near center: (%d, %d)\n", i, new_x, new_y);
		info_buff_buff[i].x = new_x;
		info_buff_buff[i].y = new_y;

		clusters_buff1[0].push_back({ Point{(double)new_x, (double)new_y, buff_width, buff_height} });//非聚类中心buff存入
    }

    writefile->get_buff_buff_info(info_buff_buff,buff_buff_cluster_num,cluster_num);
   // cout<<"im ok1"<<endl;
    writefile->get_net_info();
    writefile->write("solution.def");


    // for(int i=0;i<buff_buff_cluster_num+cluster_num;i++)
    // {
    //     cout<<writefile->my_net_info[i].connect_comp.size()<<endl;

    // }


    // for(int i=0;i<total_cluster_time;i++)
    // {
    //     cout<<"buff"<<writefile->my_buff_buff_info[i].buffname<<endl;
    //     cout<<"connect_buff_num"<<writefile->my_buff_buff_info[i].connect_buff_num<<endl;
    //     for(int j=0;j<writefile->my_buff_buff_info[i].connect_buff.size();j++)
    //     {
    //         cout<<writefile->my_buff_buff_info[i].connect_buff[j].buffname<<endl;

    //     }
    // }




    // writefile->get_net_info();

    // writefile->write("my_soluton.def");

    // for(int i=0;i<writefile->NETSNUM;i++)
    // {
    //     cout<<"net"<<writefile->my_net_info[i].name<<endl;
    //     for(int j=0;j<writefile->my_net_info[i].connect_comp.size();j++)
    //     {
    //         cout<<writefile->my_net_info[i].connect_comp[j]<<endl;
    //     }
    // }

    






    delete[] info_buff;
    delete[] labels;
    delete writefile;
    delete myfile.myffdot.data;
    return 0;
}
