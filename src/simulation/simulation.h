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

// Function declarations
SimulationState *simulation_init(void);
void simulation_cleanup(SimulationState *state);
void simulation_update(SimulationState *state);
void simulation_handle_input(SimulationState *state, SDL_Event *event);

#endif // SIMULATION_H
