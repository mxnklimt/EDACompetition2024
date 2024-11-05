#define _CRT_SECURE_NO_WARNINGS
#ifndef READFILE_H_
#define READFILE_H_

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include "ffdot.h"
using namespace std;

class readedafile
{
public:
    double net_unit_r = 0;
    double net_unit_c = 0;
    double max_net_rc = 0;
    int max_fanout = 0;
    double buffer_delay = 0;
    string my_constraintstxt;
    string my_problemdef;
    ifstream read_constraintstxt;
    ifstream read_problemdef;
    ffdot myffdot;
    int get_constraintstxt(void);
    int get_ffdot(void);
    void setfilename(string constraintstxt, string problemdef);
};

#endif
#pragma once
