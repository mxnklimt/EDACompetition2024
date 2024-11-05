#include "readfile.h"
#include "writefile.h"
#include "ffdot.h"
#include <algorithm> 
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>
#include "total_cluster.h"
#include <numeric>




int main(int argc, char **argv)
{
 //   if (argc != 3)
	//{
	//	printf("Usage:<constraints.txt> <problem.def>\n");
	//	return -1;
	//}
    srand(time(0));
    readedafile myfile;

   // myfile.setfilename(argv[1], argv[2]);
    myfile.setfilename("constraints.txt", "problem.def");
    myfile.get_constraintstxt();
    myfile.get_ffdot();
    
    std::vector<int> ffdot_area_x = { myfile.myffdot.area.area1.x, myfile.myffdot.area.area2.x, myfile.myffdot.area.area3.x, myfile.myffdot.area.area4.x };
    int ffdot_area_x_min = *std::min_element(ffdot_area_x.begin(), ffdot_area_x.end());//x_min
    int ffdot_area_x_max = *std::max_element(ffdot_area_x.begin(), ffdot_area_x.end());//x_max
    std::vector<int> ffdot_area_y = { myfile.myffdot.area.area1.y, myfile.myffdot.area.area2.y, myfile.myffdot.area.area3.y, myfile.myffdot.area.area4.y };
    int ffdot_area_y_min = *std::min_element(ffdot_area_y.begin(), ffdot_area_y.end());//y_min
    int ffdot_area_y_max = *std::max_element(ffdot_area_y.begin(), ffdot_area_y.end());//y_max


    const int size = static_cast<int>(myfile.myffdot.numofdot); //Number of Point
    const int dim = 2;   //Dimension of feature
    const int cluster_num = ceil((float)size/(myfile.max_fanout-1)); //ff_Cluster number
    const int buff_buff_cluster_num = ceil((float)cluster_num/(myfile.max_fanout-1)); //buff Cluster number


    int* labels = new int[size];


    vector<int> buff_cluster_num;
    int count_cluster=size;
    while(count_cluster>myfile.max_fanout-1)//循环计算要多少个buff聚类
    {
        count_cluster=ceil((float)count_cluster/(myfile.max_fanout-1));
        buff_cluster_num.emplace_back(count_cluster);
    }
    buff_cluster_num.emplace_back(1);//最后一个buff连接之前buff
    


    int total_cluster_time = buff_cluster_num.size();//计算要创建多个buff_buff_info
    //cout<<"total_cluster_time"<<total_cluster_time<<endl;
    // for(int i=0;i<total_cluster_time;i++)
    // {
    //     cout<<buff_cluster_num[i]<<endl;
    // }
    total_cluster* mytotal_cluster=new total_cluster(
    size,
    myfile.max_fanout,
    myfile.myffdot.my_ffsize.x,
    myfile.myffdot.my_ffsize.y,
    myfile.myffdot.my_buffsize.x,
    myfile.myffdot.my_buffsize.y,
    ffdot_area_x_max,
    ffdot_area_x_min,
    ffdot_area_y_max,
    ffdot_area_y_min
    );
    mytotal_cluster->total_kmeans(buff_cluster_num,myfile.myffdot.data);
    int clustercount=std::accumulate(buff_cluster_num.begin(), buff_cluster_num.end(), 0);
    // for(int i=0;i<clustercount-1;i++)
    // {
    //     cout <<mytotal_cluster->clusters_data[i].size()<< endl;
    // }
    writeedafile* mywriteedafile=new writeedafile(size,clustercount-1,myfile.max_fanout,&myfile);
    mywriteedafile->write("solution.def",mytotal_cluster->clusters_data,myfile.myffdot.data,buff_cluster_num);

    delete mytotal_cluster;
    delete myfile.myffdot.data;
    delete mywriteedafile;
    return 0;
}
