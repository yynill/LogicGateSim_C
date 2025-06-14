#ifndef NODEGROUP_H
#define NODEGROUP_H
#pragma once



#include <assert.h>
#include <SDL2/SDL.h>

#include "DataStructures/DynamicArray.h"

typedef struct NodeGroup {
    DynamicArray *inputs; 
    DynamicArray *outputs;
    DynamicArray *nodes;
    DynamicArray *connections;
    const char *name;
    SDL_Rect rect;
} NodeGroup;


#endif // NODEGROUP_H
