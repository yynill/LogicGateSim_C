#include "point.h"


void point_print(SDL_Point *p) {
    printf("(%d, %d)\n", p->x, p->y);
}

int point_cross(SDL_Point *a, SDL_Point *b) {
    return a->x * b->y - a->y * b->x;
}

SDL_Point point_add(SDL_Point *a, SDL_Point *b) {
    SDL_Point res = {a->x + b->x, a->y + b->y};
    return res;
}

SDL_Point point_subtract(SDL_Point *a, SDL_Point *b) {
    SDL_Point res = {a->x - b->x, a->y - b->y};
    return res;
}

SDL_Point point_multiply(SDL_Point *a, double k) {
    SDL_Point res = {(int)(a->x * k), (int)(a->y * k)};
    return res;
}

SDL_Point point_divide(SDL_Point *a, double k) {
    SDL_Point res = {(int)(a->x / k), (int)(a->y / k)};
    return res;
}

int point_orient(SDL_Point *a, SDL_Point *b, SDL_Point *c) {
    SDL_Point ab = point_subtract(b, a);
    SDL_Point ac = point_subtract(c, a);
    return point_cross(&ab, &ac);
}

// check if two segments lines intersect
int segment_intersection(SDL_Point *a, SDL_Point *b, SDL_Point *c, SDL_Point *d, SDL_Point *out) {
    double oa = point_orient(c, d, a);
    double ob = point_orient(c, d, b);
    double oc = point_orient(a, b, c);
    double od = point_orient(a, b, d);

    // Proper intersection exists iff signs are opposite
    if (oa * ob < 0 && oc * od < 0) {
        double t = oa / (oa - ob);
        SDL_Point diff = point_subtract(b, a);
        SDL_Point scaled = point_multiply(&diff, t);
        *out = point_add(a, &scaled);
        return 1;
    }
    return 0;
}