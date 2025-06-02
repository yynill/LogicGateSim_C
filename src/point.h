
#include <stdio.h>

typedef struct Point
{
    int x;
    int y;
} Point;

void point_print(Point p);
int point_cross(Point a, Point b);
Point point_add(Point a, Point b);
Point point_subtract(Point a, Point b);
Point point_multiply(Point a, double k);
Point point_divide(Point a, double k);
int point_orient(Point a, Point b, Point c);
int segment_intersection(Point a, Point b, Point c, Point d, Point *out);