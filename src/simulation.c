#include "simulation.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "DynamicArray.h"

SimulationState *simulation_init(void)
{
    SimulationState *state = malloc(sizeof(SimulationState));
    if (!state)
        return NULL;

    state->is_running = 1;

    state->nodes = array_create(16, sizeof(Node));
    state->connections = array_create(16, sizeof(Connection));

    // Initialize input state
    state->input.mouse_down = 0;
    state->input.mouse_up = 0;
    state->input.mouse_x = 0;
    state->input.mouse_y = 0;

    return state;
}

void simulation_cleanup(SimulationState *state)
{
    if (state)
    {
        array_free(state->nodes);
        array_free(state->connections);
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

    case SDL_MOUSEMOTION:
        state->input.mouse_x = event->motion.x;
        state->input.mouse_y = event->motion.y;
        break;

    case SDL_MOUSEBUTTONDOWN:
        state->input.mouse_down = 1;
        state->input.mouse_x = event->button.x;
        state->input.mouse_y = event->button.y;
        break;

    case SDL_MOUSEBUTTONUP:
        state->input.mouse_down = 0;
        state->input.mouse_x = event->button.x;
        state->input.mouse_y = event->button.y;
        break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        switch (event->key.keysym.sym)
        {
        case SDLK_ESCAPE:
            state->is_running = 0;
            break;
        }
        break;
    }
    }
}

void simulation_update(SimulationState *state)
{
    if (state->input.mouse_down)
    {
        Node node = {
            .inputs = {0, 0},
            .outputs = {0, 0},
            .operation = nullGate,
        };
        array_add(state->nodes, &node);
        state->input.mouse_down = 0;
        printf("Number of nodes: %d\n", state->nodes->size);
    }
}