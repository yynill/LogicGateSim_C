#include "simulation.h"
#include "../main.h"
#include <stdio.h>

SimulationState *simulation_init(void)
{
    SimulationState *state = malloc(sizeof(SimulationState));
    if (!state)
        return NULL;

    state->is_running = 1;

    // Initialize input state
    state->input.up = 0;
    state->input.down = 0;
    state->input.left = 0;
    state->input.right = 0;
    state->input.a = 0;
    state->input.b = 0;
    state->input.start = 0;
    state->input.select = 0;

    return state;
}

void simulation_cleanup(SimulationState *state)
{
    if (state)
    {
        free(state);
    }
}

void simulation_handle_input(SimulationState *state, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_QUIT:
        state->is_running = 0;
        break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        int is_pressed = (event->type == SDL_KEYDOWN);
        switch (event->key.keysym.sym)
        {
        case SDLK_ESCAPE:
            state->is_running = 0;
            break;
        case KEY_UP:
            state->input.up = is_pressed;
            break;
        case KEY_DOWN:
            state->input.down = is_pressed;
            break;
        case KEY_LEFT:
            state->input.left = is_pressed;
            break;
        case KEY_RIGHT:
            state->input.right = is_pressed;
            break;
        case KEY_A:
            state->input.a = is_pressed;
            break;
        case KEY_B:
            state->input.b = is_pressed;
            break;
        case KEY_START:
            state->input.start = is_pressed;
            break;
        case KEY_SELECT:
            state->input.select = is_pressed;
            break;
        }
    }
    break;
    }
}

void simulation_update(SimulationState *state)
{
    // Update logic will be handled by the renderer
}