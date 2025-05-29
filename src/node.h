#ifndef NODE_H
#define NODE_H

#include <SDL2/SDL.h>
#include "DynamicArray.h"

typedef int (*Operation)(int, int);

typedef struct Pin {
    int x, y; // rel pos to node
} Pin;

typedef struct Node
{
    DynamicArray *inputs;
    DynamicArray * outputs;
    Operation operation;
    const char *name;
    SDL_Rect rect;
} Node;

typedef struct Connection
{
    Node *from;
    Node *to;
} Connection;

// node operations

static inline int nullGate(int a, int b) { (void)a; (void)b; return 0; } // always returns 0
static inline int light_output(int a, int b) {(void)b; return a; }       // discards b, only uses a
static inline int andGate(int a, int b) { return a && b; }
static inline int orGate(int a, int b) { return a || b; }
static inline int notGate(int a, int b) { (void)b; return !a; }          // discards b, only uses a
static inline int norGate(int a, int b) { return !(a || b); }
static inline int nandGate(int a, int b) { return !(a && b); }

// functions 
Node *create_node(int num_inputs, int num_outputs, Operation op, SDL_Rect rect, const char *name);

#endif // NODE_H