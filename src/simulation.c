#include "simulation.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "DynamicArray.h"
#include <assert.h>

SimulationState *simulation_init(void) {
    SimulationState *state = malloc(sizeof(SimulationState));
    if (!state)
        return NULL;

    state->is_running = 1;

    state->nodes = array_create(16, sizeof(Node));
    assert(state->nodes != NULL && "Failed to create nodes array");

    state->connections = array_create(16, sizeof(Connection));
    assert(state->connections != NULL && "Failed to create connections array");

    state->buttons = array_create(16, sizeof(Button));
    assert(state->buttons != NULL && "Failed to create buttons array");

    Button add_node_button = {
        .rect = {10, 10, NODE_WIDTH, NODE_HEIGHT},
        .path = "assets/images/and.png",
        .function_data = andGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &add_node_button);

    Button or_node_button = {
        .rect = {80, 10, NODE_WIDTH, NODE_HEIGHT},
        .path = "assets/images/or.png",
        .function_data = orGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &or_node_button);

    Button not_node_button = {
        .rect = {150, 10, NODE_WIDTH, NODE_HEIGHT},
        .path = "assets/images/not.png",
        .function_data = notGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &not_node_button);

    Button nor_node_button = {
        .rect = {220, 10, NODE_WIDTH, NODE_HEIGHT},
        .path = "assets/images/nor.png",
        .function_data = norGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &nor_node_button);

    Button nand_node_button = {
        .rect = {290, 10, NODE_WIDTH, NODE_HEIGHT},
        .path = "assets/images/nand.png",
        .function_data = nandGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &nand_node_button);

    Button light_button = {
        .rect = {380, 10, NODE_WIDTH, NODE_HEIGHT},
        .path = "assets/images/light_on.png",
        .function_data = light_output,
        .on_press = add_node,
    };
    array_add(state->buttons, &light_button);

    Button switch_button = {
        .rect = {450, 10, NODE_WIDTH, NODE_HEIGHT},
        .path = "assets/images/switch_off.png",
        .function_data = light_output,
        .on_press = add_node,
    };
    array_add(state->buttons, &switch_button);

    Button connection_button = {
        .rect = {WINDOW_WIDTH - 50, 10, 30, 30},
        .path = "assets/images/cable.png",
        .function_data = nandGate,
        .on_press = connection_mode,
    };
    array_add(state->buttons, &connection_button);

    // Initialize input state
    state->input.mouse_down = 0;
    state->input.mouse_up = 0;
    state->input.mouse_x = 0;
    state->input.mouse_y = 0;
    state->input.is_dragging = 0;
    state->input.drag_offset_x = 0;
    state->input.drag_offset_y = 0;
    state->dragged_node = NULL;
    state->first_connection_node = NULL;

    assert(state->nodes != NULL && "Nodes array should be initialized");
    assert(state->connections != NULL && "Connections array should be initialized");
    assert(state->buttons != NULL && "Buttons array should be initialized");
    assert(state->buttons->size > 0 && "Should have at least one button");
    assert(state->is_running == 1 && "Simulation should be running after init");

    return state;
}

void simulation_cleanup(SimulationState *state) {
    assert(state != NULL && "Cannot cleanup NULL state");

    if (state)
    {
        array_free(state->nodes);
        array_free(state->connections);
        free(state);
    }
}

void simulation_handle_input(SimulationState *state, SDL_Event *event) {
    assert(state != NULL && "Cannot handle input with NULL state");
    assert(event != NULL && "Cannot handle NULL event");

    switch (event->type)
    {
    case SDL_QUIT:
        state->is_running = 0;
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (event->button.button == SDL_BUTTON_LEFT)
        {

            state->input.mouse_down = 1;
            state->input.mouse_x = event->button.x;
            state->input.mouse_y = event->button.y;
            check_click_pos(state);
            break;
        }

    case SDL_MOUSEMOTION:
        state->input.mouse_x = event->motion.x;
        state->input.mouse_y = event->motion.y;
        
        if (state->input.mouse_down) state->input.is_dragging = 1;
        break;

    case SDL_MOUSEBUTTONUP:
        if (event->button.button == SDL_BUTTON_LEFT)
        {

            state->input.mouse_down = 0;
            state->input.mouse_x = event->button.x;
            state->input.mouse_y = event->button.y;
            state->input.is_dragging = 0;
            state->dragged_node = NULL;
            break;
        }

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

void connection_mode(SimulationState *state, void *function_data) {
    assert(state != NULL && "Cannot add node to NULL state");
    assert(function_data != NULL && "Cannot add node with NULL function data");

    state->connection_mode = !state->connection_mode;
    state->first_connection_node = NULL;
}

void add_node(SimulationState *state, void *function_data) {
    assert(state != NULL && "Cannot add node to NULL state");
    assert(function_data != NULL && "Cannot add node with NULL function data");
    assert(state->nodes != NULL && "Nodes array must be initialized");

    if (state->connection_mode)
    {
        return;
    }
    

    Button *button = (Button *)function_data;
    Operation op = *(Operation *)button->function_data;

    SDL_Rect node_rect = {
        .x = 100,
        .y = 100,
        .w = NODE_WIDTH,
        .h = NODE_HEIGHT};

    Node node = {
        .inputs = {0, 0},
        .outputs = {0, 0},
        .operation = op,
        .path = button->path,
        .rect = node_rect,
    };
    array_add(state->nodes, &node);

    assert(state->nodes->size > 0 && "Node should be added to array");
}

void check_click_pos(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");
    assert(state->nodes != NULL && "Nodes array must be initialized");
    assert(state->buttons != NULL && "Buttons array must be initialized");

    if (state->input.mouse_down)
    {
        for (int i = 0; i < state->buttons->size; i++)
        {
            Button *button = array_get(state->buttons, i);
            assert(button != NULL && "Button should not be NULL");
            if (state->input.mouse_x >= button->rect.x && state->input.mouse_x <= button->rect.x + button->rect.w &&
                state->input.mouse_y >= button->rect.y && state->input.mouse_y <= button->rect.y + button->rect.h)
            {
                button->on_press(state, button);
            }
        }

        for (int i = 0; i < state->nodes->size; i++)
        {
            Node *node = array_get(state->nodes, i);
            if (state->input.mouse_x >= node->rect.x && state->input.mouse_x <= node->rect.x + node->rect.w &&
                state->input.mouse_y >= node->rect.y && state->input.mouse_y <= node->rect.y + node->rect.h)
            {
                if (state->connection_mode)
                {
                    if (state->first_connection_node == NULL) state->first_connection_node = node;
                    else { 
                        Connection con ={
                            .from = state->first_connection_node, 
                            .to = node,
                        };
                        array_add(state->connections, &con);
                        state->first_connection_node = NULL;
                    }
                }
                else
                {
                    state->input.drag_offset_x = state->input.mouse_x - node->rect.x;
                    state->input.drag_offset_y = state->input.mouse_y - node->rect.y;
                    state->input.is_dragging = 1;
                    state->dragged_node = node;
                }
                break;
            }
        }
    }
}

void simulation_update(SimulationState *state)
{
    if (state->input.is_dragging && state->dragged_node != NULL)
    {
        state->dragged_node->rect.x = state->input.mouse_x - state->input.drag_offset_x;
        state->dragged_node->rect.y = state->input.mouse_y - state->input.drag_offset_y;
    }
}