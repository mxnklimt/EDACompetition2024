#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include "ffdot.h"
#include "readfile.h"
#include <vector>
#include <cmath>
using namespace std;
struct Point {
    double x, y;
    int type;//0为ff 1为buff
    string name;
    int width;
    int height;
};
class total_cluster
{
public:
    int ffnum;
    int maxfanout;
    int ffsizex;
    int ffsizey;
    int buffsizex;
    int buffsizey;
    int area_x_max;
    int area_x_min;
    int area_y_max;
    int area_y_min;
    std::vector<std::vector<Point>> clusters_data;
    total_cluster(int myffnum,int mymaxfanout,int myffsizex,int myffsizey,int mybuffsizex,int mybuffsizey,int myarea_x_max,int myarea_x_min,int myarea_y_max,int myarea_y_min);

    double distance(const Point& p1, const Point& p2);
    vector<Point> kmeans(vector<Point>& points, int k, int max_iterations,int numofcomp,std::vector<int> &labels,int max_cluster_point);
    void total_kmeans(vector<int>& buff_cluster_num,double* data) ;

};