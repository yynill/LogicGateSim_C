#ifndef NODE_H
#define NODE_H
#pragma once

#include <SDL2/SDL.h>
#include <assert.h>
#include "DataStructures/DynamicArray.h"

typedef int (Operation)(int, int);
typedef struct SimulationState SimulationState;

typedef struct Node
{
    DynamicArray *inputs;  // pins
    DynamicArray *outputs; // pins
    Operation *operation;
    const char *name;
    SDL_Rect rect;
} Node;

typedef struct Pin {
    int x, y; // rel pos to node
    int is_input;
    Node *parent_node; 
    int state;
} Pin;

/* basic node operations */

static inline int nullGate(int a, int b) { (void)a; (void)b; return 0; } 

// Basic logic gates
static inline int notGate(int a, int b)  { (void)b; return !a; }
static inline int andGate(int a, int b)  { return a && b; }
static inline int nandGate(int a, int b) { return !(a && b); }
static inline int orGate(int a, int b)   { return a || b; }
static inline int norGate(int a, int b)  { return !(a || b); }
static inline int xorGate(int a, int b)  { return a != b; }
static inline int xnorGate(int a, int b) { return a == b; }

/* todo: dont see the value in implementing them 

// Constant gates
static inline int falseGate(int a, int b) { (void)a; (void)b; return 0; }
static inline int trueGate(int a, int b)  { (void)a; (void)b; return 1; }

// Implication gates
static inline int implyFirstGate(int a, int b)  { return !a || b; } // A ⇒ B
static inline int nimplyFirstGate(int a, int b) { return a && !b; } // A ↛ B
static inline int implySecondGate(int a, int b) { return !b || a; } // B ⇒ A
static inline int nimplySecondGate(int a, int b){ return b && !a; } // B ↛ A

// Identity gates
static inline int firstGate(int a, int b)     { (void)b; return a; }
static inline int notFirstGate(int a, int b)  { (void)b; return !a; }
static inline int secondGate(int a, int b)    { (void)a; return b; }
static inline int notSecondGate(int a, int b) { (void)a; return !b; }
*/

// functions
void print_node(Node *node);
void run_node(Node *node);

Pin *create_pin(int x, int y, int ii, Node *parent_node);
Node *create_node(int num_inputs, int num_outputs, Operation op, SDL_Rect rect, const char *name);
void free_node(Node *node);
#endif // NODE_H