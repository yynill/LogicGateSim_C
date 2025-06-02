#include "point.h"


void point_print(Point p) {
    printf("(%d, %d)\n", p.x, p.y);
}

int point_cross(Point a, Point b) {
    return a.x * b.y - a.y * b.x;
}

Point point_add(Point a, Point b) {
    Point res = {a.x + b.x, a.y + b.y};
    return res;
}

Point point_subtract(Point a, Point b) {
    Point res = {a.x - b.x, a.y - b.y};
    return res;
}

Point point_multiply(Point a, double k) {
    Point res = {(int)(a.x * k), (int)(a.y * k)};
    return res;
}

Point point_divide(Point a, double k) {
    Point res = {(int)(a.x / k), (int)(a.y / k)};
    return res;
}


int point_orient(Point a, Point b, Point c) {
    return point_cross(point_subtract(b, a), point_subtract(c, a));
}

// check if two segments lines intersect
int segment_intersection(Point a, Point b, Point c, Point d, Point *out) {
    double oa = point_orient(c, d, a);
    double ob = point_orient(c, d, b);
    double oc = point_orient(a, b, c);
    double od = point_orient(a, b, d);

    // Proper intersection exists iff signs are opposite
    if (oa * ob < 0 && oc * od < 0) {
        double t = oa / (oa - ob);
        Point diff = point_subtract(b, a);
        Point scaled = point_multiply(diff, t);
        *out = point_add(a, scaled);
        return 1;
    }
    return 0;
}