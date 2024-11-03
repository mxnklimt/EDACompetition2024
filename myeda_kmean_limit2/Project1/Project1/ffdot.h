#ifndef FFDOT_H_
#define FFDOT_H_

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
using namespace std;
struct two_coordinates
{
    int x;
    int y;
};
struct comp_size
{
    int x;
    int y;
};
struct DIEAREA
{
    two_coordinates area1;
    two_coordinates area2;
    two_coordinates area3;
    two_coordinates area4;
};
struct ffcell
{
    int x;
    int y;
};
class ffdot
{
public:
    double UNITS_DISTANCE = 0;
    DIEAREA area;
    comp_size my_ffsize;
    comp_size my_buffsize;
    two_coordinates clk_coordinate;
    double numofdot = 0; //number of dot
    //ffcell *my_ffcell; //坐标
    double* data;

};
// 定义一个新的结构体 Point，包含坐标和尺寸
//struct comp_Point {
//    int x;
//    int y;
//    int width;
//    int height;
//};
struct Point {
    double x, y;
    int width;
    int height;
};
#endif
