#include "total_cluster.h"
#include "findNonOverlappingPosition.h"
#include <numeric>
double total_cluster::distance(const Point& p1, const Point& p2) {
    return (((p1.x - p2.x))+ ((p1.y - p2.y)) );
}


vector<Point> total_cluster::kmeans(vector<Point>& points, int k, int max_iterations,int numofcomp,std::vector<int> &labels,int max_cluster_point) {
    vector<Point> centroids(k);
    vector<int> cluster_sizes(k, 0);
    vector<vector<Point>> clusters(k);

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
            labels.emplace_back(best_cluster);
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
    


    return centroids;
}


void total_cluster::total_kmeans(vector<int>& buff_cluster_num,double* data) {
    int clustercount=std::accumulate(buff_cluster_num.begin(), buff_cluster_num.end(), 0);
    std::vector<std::vector<Point>> allpoint(1);
    vector<Point> ffpoints(ffnum);
    std::vector<std::vector<Point>> clusters_buff(clustercount-1);
    cout<<"------------------ff kmeans begin--------------------"<<endl;
    //clusters_buff=new std::vector<std::vector<Point>>[clustercount-1];
    for (int i = 0; i < ffnum; i++) {
        ffpoints[i].type=0;
        ffpoints[i].name=to_string(i+1);
        ffpoints[i].x=data[i*2];
        ffpoints[i].y=data[i*2+1];
        ffpoints[i].width = (ffsizex);
        ffpoints[i].height = (ffsizey);
        allpoint[0].emplace_back(ffpoints[i]);
    }  

    int max_iterations = 5;
    std::vector<std::vector<int>> labels(buff_cluster_num.size()-1+1); 
    vector<Point> centroids = kmeans(ffpoints, buff_cluster_num[0], max_iterations,ffnum,labels[0], maxfanout - 1);//// max_cluster_point = myfile.max_fanout - 1每个聚类中最多元素数量，即每个buff连接的数量   

    for (int i = 0; i < ffnum; ++i)
    {
        Point buff;
        int cluster_id = labels[0][i];
        buff.x = static_cast<int>(data[i * 2 + 0]);
        buff.y = static_cast<int>(data[i * 2 + 1]);
        buff.width = static_cast<int>(ffsizex);
        buff.height = static_cast<int>(ffsizey);
        buff.type = 0;
        buff.name = to_string(i+1);
        clusters_buff[cluster_id].emplace_back(buff);
    }//Put the data into std::vector<std::vector<Point>> clusters(cluster_num);

    // 计算每个簇的中心位置并放置缓冲区
    for (int i = 0; i < buff_cluster_num[0]; ++i)
    {
        Point buff;
        int sum_x = 0, sum_y = 0;


        for (const auto& point : clusters_buff[i])
        {
            sum_x += point.x;
            sum_y += point.y;
        }
        int center_x = sum_x / clusters_buff[i].size(); // 簇的中心x坐标
        int center_y = sum_y / clusters_buff[i].size(); // 簇的中心y坐标

        //cluster_buff[i]可以获得当前buff2连接的所有buff1的数据
        int buff_width = (buffsizex);
        int buff_height = (buffsizey);
		auto [new_x, new_y] = findNonOverlappingPosition(center_x, center_y, ffsizex, ffsizey, 
            allpoint, area_x_max, area_x_min, area_y_max, area_y_min);

		buff.x = new_x;
		buff.y = new_y;
        buff.width = (buffsizex);
        buff.height = (buffsizey);
        buff.type = 1;
        buff.name = to_string(i);
        allpoint[0].push_back(buff);
		clusters_buff[i].emplace_back(buff);//聚类中心buff存入
        
    }
    cout<<"------------------ff kmeans ok-----------------------"<<endl;
    


    int clusterpoint=0;
    for(int i=0;i<buff_cluster_num.size()-2;i++)
    {
        cout<<"------------------buff kmeans begin--------------------"<<endl;
        int beforclusternum=buff_cluster_num[i];
        
        clusterpoint+=beforclusternum;
        vector<Point> buffpoints(beforclusternum);
        //std::fill(buffpoints.begin(), buffpoints.end(), 0);

        for (int j = clusterpoint-beforclusternum; j < clusterpoint; j++) 
        {
            Point last=clusters_buff[j][clusters_buff[j].size()-1];
            buffpoints[j-clusterpoint+beforclusternum].type=last.type;
            buffpoints[j-clusterpoint+beforclusternum].name=last.name;
            buffpoints[j-clusterpoint+beforclusternum].x=last.x;
            buffpoints[j-clusterpoint+beforclusternum].y=last.y;
        } 
        vector<Point> centroids = kmeans(buffpoints, buff_cluster_num[i+1], max_iterations,beforclusternum,labels[i+1], maxfanout - 1);//// max_cluster_point = myfile.max_fanout - 1每个聚类中最多元素数量，即每个buff连接的数量   

        for (int k = 0; k < beforclusternum; ++k)
        {
            Point buff;
            int cluster_id = labels[i+1][k];
            buff.x = (buffpoints[k].x);
            buff.y = (buffpoints[k].y);
            buff.width = (buffsizex);
            buff.height = (buffsizey);
            buff.type = 1;
            buff.name = buffpoints[k].name;
            clusters_buff[cluster_id+clusterpoint].emplace_back(buff);
        }

        // 计算每个簇的中心位置并放置缓冲区
        for (int z = 0; z < buff_cluster_num[i+1]; ++z)
        {
            Point buff;
            int sum_x = 0, sum_y = 0;

            for (const auto& point : clusters_buff[z+clusterpoint])
            {
                sum_x += point.x;
                sum_y += point.y;
            }
            int center_x = sum_x / clusters_buff[z+clusterpoint].size(); // 簇的中心x坐标
            int center_y = sum_y / clusters_buff[z+clusterpoint].size(); // 簇的中心y坐标

            //cluster_buff[i]可以获得当前buff2连接的所有buff1的数据
            int buff_width = (buffsizex);
            int buff_height = (buffsizey);
            auto [new_x, new_y] = findNonOverlappingPosition(center_x, center_y, buffsizex, buffsizey, 
            allpoint, area_x_max, area_x_min, area_y_max, area_y_min);

            buff.x = new_x;
            buff.y = new_y;
            buff.width = (buffsizex);
            buff.height = (buffsizey);
            buff.type = 1;
            buff.name = to_string(z+clusterpoint);
            clusters_buff[z+clusterpoint].emplace_back(buff);//聚类中心buff存入
            allpoint[0].emplace_back(buff);
        }
        cout<<"------------------buff kmeans ok-----------------------"<<endl;
    }
    clusters_data=clusters_buff;
    //cout<<allpoint[0].size()<<endl;
}

total_cluster::total_cluster(int myffnum,int mymaxfanout,int myffsizex,int myffsizey,int mybuffsizex,int mybuffsizey,int myarea_x_max,int myarea_x_min,int myarea_y_max,int myarea_y_min)
{
     ffnum=myffnum;
     maxfanout=mymaxfanout;
     ffsizex=myffsizex;
     ffsizey=myffsizey;
     buffsizex=mybuffsizex;
     buffsizey=mybuffsizey;
     area_x_max=myarea_x_max;
     area_x_min=myarea_x_min;
     area_y_max=myarea_y_max;
     area_y_min=myarea_y_min;    
}

