#ifndef SIMULATION_H
#define SIMULATION_H

#include <SDL2/SDL.h>
#include "DynamicArray.h"

typedef struct SimulationState
{
    int is_running;
    DynamicArray *nodes;
    DynamicArray *connections;

    // Input state
    struct
    {
        int mouse_down;
        int mouse_up;
        int mouse_x;
        int mouse_y;
    } input;
} SimulationState;

// operations
typedef int (*Operation)(int, int);
static inline int nullGate(int a, int b)
{
    (void)a;
    (void)b;
    return 0;
} // always returns 0
static inline int andGate(int a, int b) { return a && b; }
static inline int orGate(int a, int b) { return a || b; }
static inline int norGate(int a, int b) { return !(a || b); }
static inline int nandGate(int a, int b) { return !(a && b); }
static inline int notGate(int a, int b)
{
    (void)b;
    return !a;
} // discards b, only uses a

typedef struct Node
{
    int inputs[2];
    int outputs[2];
    Operation operation;
} Node;

typedef struct Connection
{
    Node *from;
    Node *to;
} Connection;

// Function declarations
SimulationState *simulation_init(void);
void simulation_cleanup(SimulationState *state);
void simulation_update(SimulationState *state);
void simulation_handle_input(SimulationState *state, SDL_Event *event);

#endif // SIMULATION_H
