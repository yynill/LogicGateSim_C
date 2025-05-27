#ifndef SIMULATION_H
#define SIMULATION_H

#include <SDL2/SDL.h>

typedef struct SimulationState
{
    int is_running;

    // Input state
    struct
    {
        int up;
        int down;
        int left;
        int right;
        int a;
        int b;
        int start;
        int select;
    } input;
} SimulationState;

typedef int (*Operation)(int, int);

typedef struct Node {
    int[2] inputs;
    int[2] outputs;
    Operation operation;
} Node;

typedef struct Connection {
    Node* from;
    Node* to;
} Connection;


// operations
int andGate(int a, int b) { return a && b; }
int orGate(int a, int b) { return a || b; }
int norGate(int a, int b) { return !(a || b); }
int nandGate(int a, int b) { return !(a && b); }
int notGate(int a, int b) { (void)b; return !a; } // discards b, only uses a

// Function declarations
SimulationState *simulation_init(void);
void simulation_cleanup(SimulationState *state);
void simulation_update(SimulationState *state);
void simulation_handle_input(SimulationState *state, SDL_Event *event);

#endif // SIMULATION_H
