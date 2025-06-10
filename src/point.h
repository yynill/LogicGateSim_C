
#include <stdio.h>
#include <SDL2/SDL.h>


void point_print(SDL_Point *p);
int point_cross(SDL_Point *a, SDL_Point *b);
SDL_Point point_add(SDL_Point *a, SDL_Point *b);
SDL_Point point_subtract(SDL_Point *a, SDL_Point *b);
SDL_Point point_multiply(SDL_Point *a, double k);
SDL_Point point_divide(SDL_Point *a, double k);
int point_orient(SDL_Point *a, SDL_Point *b, SDL_Point *c);
int segment_intersection(SDL_Point *a, SDL_Point *b, SDL_Point *c, SDL_Point *d, SDL_Point *out);