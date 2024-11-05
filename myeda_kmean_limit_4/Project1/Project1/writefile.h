#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include "ffdot.h"
#include "readfile.h"
#include <vector>
#include "total_cluster.h"
using namespace std;



class writeedafile
{
public:
    double UNITS_DISTANCE = 0;
    DIEAREA area;
    comp_size my_ffsize;
    comp_size my_buffsize;
    two_coordinates clk_coordinate;
    double COMPONENTNUM = 0;
    double my_ffnum;
    double my_buffnum;
    double NETSNUM=0;
    double max_fanout = 0;
    string my_solutiondef;
    ofstream  write_solutiondef;
    writeedafile(int ffnum,int buffnum,int fanout,readedafile *myfile);

    void write(string name,std::vector<std::vector<Point>> &clusters_data,double* ffdata,vector<int> &buff_cluster_num);

};




