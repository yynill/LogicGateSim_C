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
    state->buttons = array_create(16, sizeof(Button));

    Button add_node_button = {
        .rect = {10, 10, 60, 30},
        .path = "assets/images/and.png",
        .function_data = andGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &add_node_button);

    Button or_node_button = {
        .rect = {80, 10, 60, 30},
        .path = "assets/images/or.png",
        .function_data = orGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &or_node_button);

    Button not_node_button = {
        .rect = {150, 10, 60, 30},
        .path = "assets/images/not.png",
        .function_data = notGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &not_node_button);

    Button nor_node_button = {
        .rect = {220, 10, 60, 30},
        .path = "assets/images/nor.png",
        .function_data = norGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &nor_node_button);

    Button nand_node_button = {
        .rect = {290, 10, 60, 30},
        .path = "assets/images/nand.png",
        .function_data = nandGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &nand_node_button);

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

void add_node(SimulationState *state, void *function_data)
{
    Button *button = (Button *)function_data;
    Operation op = *(Operation *)button->function_data;

    SDL_Rect node_rect = {
        .x = 100,
        .y = 100,
        .w = 60,
        .h = 30};

    Node node = {
        .inputs = {0, 0},
        .outputs = {0, 0},
        .operation = op,
        .path = button->path,
        .rect = node_rect,
    };
    array_add(state->nodes, &node);
}

void simulation_update(SimulationState *state)
{
    if (state->input.mouse_down)
    {
        for (int i = 0; i < state->buttons->size; i++)
        {
            Button *button = array_get(state->buttons, i);
            if (state->input.mouse_x >= button->rect.x && state->input.mouse_x <= button->rect.x + button->rect.w &&
                state->input.mouse_y >= button->rect.y && state->input.mouse_y <= button->rect.y + button->rect.h)
            {
                button->on_press(state, button);
                printf("Button Path: %s\n", button->path);
                printf("Number of nodes: %d\n", state->nodes->size);
            }
            state->input.mouse_down = 0;
        }
    }
}