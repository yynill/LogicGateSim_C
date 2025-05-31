#include "simulation.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "DynamicArray.h"
#include "renderer.h"
#include <assert.h>

void null_function(SimulationState *state, void *function_data){
    (void)state;
    (void)function_data;
    printf("todo\n"); // todo: delete when others implememtned - just palce holder for now.
    return;
}

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
        .rect = {10, 10, BUTTON_WIDTH, BUTTON_HEIGHT},
        .name = "AND",
        .function_data = andGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &add_node_button);

    Button or_node_button = {
        .rect = {80, 10, BUTTON_WIDTH, BUTTON_HEIGHT},
        .name = "OR",
        .function_data = orGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &or_node_button);

    Button not_node_button = {
        .rect = {150, 10, BUTTON_WIDTH, BUTTON_HEIGHT},
        .name = "NOT",
        .function_data = notGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &not_node_button);

    Button nor_node_button = {
        .rect = {220, 10, BUTTON_WIDTH, BUTTON_HEIGHT},
        .name = "NOR",
        .function_data = norGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &nor_node_button);

    Button nand_node_button = {
        .rect = {290, 10, BUTTON_WIDTH, BUTTON_HEIGHT},
        .name = "NAND",
        .function_data = nandGate,
        .on_press = add_node,
    };
    array_add(state->buttons, &nand_node_button);

    Button light_button = {
        .rect = {380, 10, BUTTON_WIDTH, BUTTON_HEIGHT},
        .name = "LIGHT",
        .function_data = light_output,
        .on_press = add_node,
    };
    array_add(state->buttons, &light_button);

    Button switch_button = {
        .rect = {450, 10, BUTTON_WIDTH, BUTTON_HEIGHT},
        .name = "SWITCH",
        .function_data = light_output,
        .on_press = add_node,
    };
    array_add(state->buttons, &switch_button);

    Button step_back_button = {
        .rect = {WINDOW_WIDTH - 4 * (BUTTON_HEIGHT + 10), 10, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2},
        .name = "/assets/images/step_back.png",
        .function_data = light_output,
        .on_press = null_function,
    };
    array_add(state->buttons, &step_back_button);

    Button play_button = {
        .rect = {WINDOW_WIDTH - 3 * (BUTTON_HEIGHT + 10), 10, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2},
        .name = "/assets/images/play.png",
        .function_data = light_output,
        .on_press = null_function,
    };
    array_add(state->buttons, &play_button);

    Button step_forth_button = {
        .rect = {WINDOW_WIDTH - 2 * (BUTTON_HEIGHT + 10), 10, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2},
        .name = "/assets/images/step_forth.png",
        .function_data = light_output,
        .on_press = one_step,
    };
    array_add(state->buttons, &step_forth_button);

    Button reload_button = {
        .rect = {WINDOW_WIDTH - 1 * (BUTTON_HEIGHT + 10) - 10, 10, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2},
        .name = "/assets/images/reload.png",
        .function_data = light_output,
        .on_press = null_function,
    };
    array_add(state->buttons, &reload_button);

    state->input.mouse_down = 0;
    state->input.mouse_up = 0;
    state->input.mouse_x = 0;
    state->input.mouse_y = 0;
    state->input.is_dragging = 0;
    state->input.drag_offset_x = 0;
    state->input.drag_offset_y = 0;
    state->dragged_node = NULL;
    state->hovered_pin = NULL;
    state->first_selected_pin = NULL;

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

    switch (event->type) {
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
        check_motion_pos(state);
        
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
        switch (event->key.keysym.sym)
        {
        case SDLK_ESCAPE:
            state->is_running = 0;
            break;
        }
        break;
    }
}

void add_node(SimulationState *state, void *function_data) {
    assert(state != NULL && "Cannot add node to NULL state");
    assert(function_data != NULL && "Cannot add node with NULL function data");
    assert(state->nodes != NULL && "Nodes array must be initialized");

    Button *button = (Button *)function_data;
    Operation op = *(Operation *)button->function_data;
    SDL_Rect node_rect = {.x = 100, .y = 100, .w = NODE_WIDTH, .h = NODE_HEIGHT};

    int num_inputs = 2;
    int num_outputs = 1;
    if (strcmp(button->name, "NOT") == 0) {
        num_inputs = 1;
    }
    if (strcmp(button->name, "SWITCH") == 0) {
        num_inputs = 0;
        node_rect.w = NODE_HEIGHT;
    }
    if (strcmp(button->name, "LIGHT") == 0) {
        num_inputs = 1;
        num_outputs = 0;
        node_rect.w = NODE_HEIGHT;
    }

    if(!insert_node(state, num_inputs, num_outputs, op, node_rect, button->name)){
        printf("filed to add node\n");
    }
    // array_add(state->nodes, node);

    assert(state->nodes->size > 0 && "Node should be added to array");
}

void check_motion_pos(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");
    assert(state->nodes != NULL && "Nodes array must be initialized");
    assert(state->buttons != NULL && "Buttons array must be initialized");
    
    state->hovered_pin = NULL;
    
    // todo: chunk based checks to not for loop over all items for mouse.
    for (int i = 0; i < state->nodes->size; i++) {
        Node *node = array_get(state->nodes, i);

        for (int n = 0; n < node->inputs->size; n++) {
            Pin *pin = (Pin *)array_get(node->inputs, n);

            if (state->input.mouse_x >= pin->x + node->rect.x && state->input.mouse_x <= pin->x + node->rect.x + PIN_SIZE &&
                state->input.mouse_y >= pin->y + node->rect.y && state->input.mouse_y <= pin->y + node->rect.y + PIN_SIZE) {
                state->hovered_pin = pin;
                return;
            }
        }

        for (int n = 0; n < node->outputs->size; n++) {
            Pin *pin = (Pin *)array_get(node->outputs, n);
            if (state->input.mouse_x >= pin->x + node->rect.x && state->input.mouse_x <= pin->x + node->rect.x + PIN_SIZE &&
                state->input.mouse_y >= pin->y + node->rect.y && state->input.mouse_y <= pin->y + node->rect.y + PIN_SIZE) {
                state->hovered_pin = pin;
                return;
            }
        }
    }
}

void check_click_pos(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");
    assert(state->nodes != NULL && "Nodes array must be initialized");
    assert(state->buttons != NULL && "Buttons array must be initialized");

    if (state->input.mouse_down) {
        for (int i = 0; i < state->buttons->size; i++) {
            Button *button = array_get(state->buttons, i);
            assert(button != NULL && "Button should not be NULL");
            if (state->input.mouse_x >= button->rect.x && state->input.mouse_x <= button->rect.x + button->rect.w &&
                state->input.mouse_y >= button->rect.y && state->input.mouse_y <= button->rect.y + button->rect.h) {
                button->on_press(state, button);
            }
        }

        for (int i = state->nodes->size - 1; i >= 0; i--) {
            Node *node = array_get(state->nodes, i);
            if (state->input.mouse_x >= node->rect.x && state->input.mouse_x <= node->rect.x + node->rect.w &&
                state->input.mouse_y >= node->rect.y && state->input.mouse_y <= node->rect.y + node->rect.h) {
                state->input.drag_offset_x = state->input.mouse_x - node->rect.x;
                state->input.drag_offset_y = state->input.mouse_y - node->rect.y;
                state->input.is_dragging = 1;
                state->dragged_node = node;
                break;
            }
        }

        if (state->hovered_pin != NULL) {
            if (state->first_selected_pin == NULL) state->first_selected_pin = state->hovered_pin;
            else if(state->first_selected_pin->is_input == state->hovered_pin->is_input) state->first_selected_pin = NULL;
            else {
                Connection new_con = {
                    .p1 = state->first_selected_pin,
                    .p2 = state->hovered_pin,
                };
                state->first_selected_pin = NULL;
                array_add(state->connections, &new_con);
                // todo: set input of node b to output of node a
            }
        }
    }
}

void one_step(SimulationState *state, void *function_data) {
    assert(state != NULL);
    (void)function_data;

    printf("one step %d\n", state->connections->size);
}

void simulation_update(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");
    if (state->input.is_dragging && state->dragged_node != NULL) {
        state->dragged_node->rect.x = state->input.mouse_x - state->input.drag_offset_x;
        state->dragged_node->rect.y = state->input.mouse_y - state->input.drag_offset_y;
    }
}