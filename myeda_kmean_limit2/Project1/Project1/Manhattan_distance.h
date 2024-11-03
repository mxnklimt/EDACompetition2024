#pragma once
#include"ffdot.h"

double Manhattan_distance(const Point& p1, const Point& p2)
{
    return (std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y));
}
double Manhattan_distance(const Point& p1,int x,int y)
{
    return (std::abs(p1.x - x) + std::abs(p1.y - y));
}
double Manhattan_distance_check_step1( )
{
    return 0;
}