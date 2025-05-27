#ifndef SIMULATION_H
#define SIMULATION_H

#include <SDL2/SDL.h>
#include "DynamicArray.h"

typedef struct SimulationState
{
    int is_running;
    DynamicArray *nodes;
    DynamicArray *connections;
    DynamicArray *buttons;

    // Input state
    struct
    {
        int mouse_down;
        int mouse_up;
        int mouse_x;
        int mouse_y;
    } input;
} SimulationState;

typedef struct Button
{
    SDL_Rect rect;
    const char *path;
    void *function_data;                                           // data for the func
    void (*on_press)(SimulationState *state, void *function_data); // Function pointer for button press action
} Button;

// operations
typedef int (*Operation)(int, int);

static inline int nullGate(int a, int b) { (void)a; (void)b; return 0; } // always returns 0
static inline int andGate(int a, int b) { return a && b; }
static inline int orGate(int a, int b) { return a || b; }
static inline int notGate(int a, int b) { (void)b; return !a; } // discards b, only uses a
static inline int norGate(int a, int b) { return !(a || b); }
static inline int nandGate(int a, int b) { return !(a && b); }
// static inline int xorGate(int a, int b) { return a != b; }
// static inline int xnorGate(int a, int b) { return a == b; }

typedef struct Node
{
    int inputs[2];
    int outputs[2];
    Operation operation;
    const char *path;
    SDL_Rect rect;
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
void add_node(SimulationState *state, void *function_data);

#endif // SIMULATION_H
