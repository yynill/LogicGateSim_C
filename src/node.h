#ifndef NODE_H
#define NODE_H

#include <SDL2/SDL.h>
#include "DynamicArray.h"

typedef int (*Operation)(int, int);
typedef struct SimulationState SimulationState;

typedef struct Node
{
    DynamicArray *inputs;  // pins
    DynamicArray *outputs; // pins
    Operation operation;
    const char *name;
    SDL_Rect rect;
} Node;

typedef struct Pin {
    int x, y; // rel pos to node
    Node *parent_node; 
} Pin;

typedef struct Connection
{
    Pin *p1;
    Pin *p2;
} Connection;

// node operations

static inline int nullGate(int a, int b) { (void)a; (void)b; return 0; } 
static inline int light_output(int a, int b) {(void)b; return a; }
static inline int andGate(int a, int b) { return a && b; }
static inline int orGate(int a, int b) { return a || b; }
static inline int notGate(int a, int b) { (void)b; return !a; }
static inline int norGate(int a, int b) { return !(a || b); }
static inline int nandGate(int a, int b) { return !(a && b); }

// functions 
int insert_node(SimulationState *state, int num_inputs, int num_outputs, Operation op, SDL_Rect rect, const char *name);

#endif // NODE_H