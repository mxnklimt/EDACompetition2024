#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include "ffdot.h"
#include "readfile.h"
#include <vector>
using namespace std;


struct ff_info
{
    string ffname;
    int clusternum;
    int x;
    int y;
};
struct buff_info
{
    string buffname;
    int clusternum;
    int x;
    int y;
};

struct ff_buff_info
{
    string buffname;
    vector<ff_info> connect_ff;
    int connect_ff_num;
    int front_clusternum;//前面的聚类号
    int back_clusternum;//后面的聚类号
    int x;
    int y;
};
struct net_info
{
    string name;
    string buffname;
    vector<string> connect_comp;
};

struct buff_buff_info
{
    string buffname;
    vector<buff_info> connect_buff;
    int connect_buff_num;
    int front_clusternum;//前面的聚类号
    int back_clusternum;//后面的聚类号
    int x;
    int y;
};
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
    double buffernamecount=0;
    double netnamecount=0;
    string my_solutiondef;
    ff_info* myff_info;
    buff_info* mybuff_info;
    ff_buff_info* my_ff_buff_info;//ff连接buff
    int my_ff_buff_num=0;
    buff_buff_info* my_buff_buff_info;//buff连接buff
    int my_buff_buff_num=0;
    net_info* my_net_info;
    ofstream  write_solutiondef;
    writeedafile(int ffnum,int ff_buffnum,int buff_buffnum,int fanout,readedafile *myfile,int buff_buff_cluster_num);
    void get_ffinfo(int* labels,double* ffcoord,int num);
    void get_ff_buff_info(ff_buff_info* info_buff,int cluster_num,int ffnum);
    void get_net_info(void);
    void write(string name);
    void get_buff_buff_info(buff_buff_info* info_buff,int cluster_num,int ff_buff_num);
    void get_buffinfo(int* labels,double* ffcoord,int num,ff_buff_info* info_buff);
};




