#include "simulation.h"

#include "DataStructures/DynamicArray.h"
#include "main.h"
#include "renderer.h"
#include "point.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

SimulationState *simulation_init(void) {
    SimulationState *state = malloc(sizeof(SimulationState));
    if (!state)
        return NULL;

    state->is_running = 1;

    state->nodes = array_create(16);
    assert(state->nodes != NULL && "Failed to create nodes array");

    state->connections = array_create(16);
    assert(state->connections != NULL && "Failed to create connections array");

    state->buttons = array_create(16);
    assert(state->buttons != NULL && "Failed to create buttons array");

    state->knife_stroke = array_create(16);
    assert(state->knife_stroke != NULL && "Failed to create knife_stroke array");

    array_add(state->buttons, create_button((SDL_Rect){10, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NOT", notGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){80, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "AND", andGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){150, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "OR", orGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){220, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "XOR", xorGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){290, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "XNOR", xnorGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){360, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NOR", norGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){430, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NAND", nandGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){550, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "LIGHT", nullGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){620, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "SWITCH", nullGate, add_node));

    array_add(state->buttons, create_button((SDL_Rect){WINDOW_WIDTH - 4 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/step_back.png", nullGate, null_function));
    array_add(state->buttons, create_button((SDL_Rect){WINDOW_WIDTH - 3 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/play.png", nullGate, toggle_play_pause));
    array_add(state->buttons, create_button((SDL_Rect){WINDOW_WIDTH - 2 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/step_forth.png", nullGate, one_step));
    array_add(state->buttons, create_button((SDL_Rect){WINDOW_WIDTH - 1 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/reload.png", nullGate, reset_sim));
    
    array_add(state->buttons, create_button((SDL_Rect){WINDOW_WIDTH - 1 * (BUTTON_HEIGHT + 10), WINDOW_HEIGHT - BUTTON_HEIGHT - 10, BUTTON_HEIGHT, BUTTON_HEIGHT}, "/assets/images/trash.png", nullGate, null_function));

    state->input.left_mouse_down = 0;
    state->input.right_mouse_down = 0;
    state->input.middle_mouse_down = 0;

    state->input.mouse_x = 0;
    state->input.mouse_y = 0;
    state->input.mouse_wheel = 0;
    state->input.drag_offset_x = 0;
    state->input.drag_offset_y = 0;

    state->input.is_node_dragging = 0;
    state->input.is_camera_dragging = 0;

    state->input.camera_x = 0;
    state->input.camera_y = 0;
    state->input.camera_zoom = 1;

    state->input.is_paused = 1;

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
        array_free_with_elements(state->nodes);
        array_free_with_elements(state->connections);
        array_free_with_elements(state->buttons);
        array_free_with_elements(state->knife_stroke);
        free(state);
    }
}

void reset_sim(SimulationState *state, void *function_data)
{
    assert(state != NULL);
    (void)function_data;
    state->should_reset = 1;
}

void simulation_update(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");

    if (!state->input.is_paused) {
        one_step(state, "");
    }
}

void null_function(SimulationState *state, void *function_data) {
    (void)state;
    (void)function_data;
    printf("todo\n"); // todo: delete when others implememtned - just palce holder for now.
    return;
}

void add_node(SimulationState *state, void *function_data) {
    assert(state != NULL && "Cannot add node to NULL state");
    assert(function_data != NULL && "Cannot add node with NULL function data");
    assert(state->nodes != NULL && "Nodes array must be initialized");

    Button *button = (Button *)function_data;
    Operation *op = (Operation *)button->function_data;
    SDL_Rect node_rect = {.x = 100, .y = 100, .w = NODE_WIDTH, .h = NODE_HEIGHT};

    int num_inputs = 2;
    int num_outputs = 1;
    if (strcmp(button->name, "NOT") == 0) {
        num_inputs = 1;
    }
    if (strcmp(button->name, "SWITCH") == 0) {
        num_inputs = 0;
        node_rect.w = SMALL_NODE_WIDTH;
    }
    if (strcmp(button->name, "LIGHT") == 0) {
        num_inputs = 1;
        num_outputs = 0;
        node_rect.w = SMALL_NODE_WIDTH;
    }

    array_add(state->nodes, create_node(num_inputs, num_outputs, op, node_rect, button->name));
}

void connection_stroke_intersection(SimulationState *state) {
    assert(state != NULL);
    assert(state->connections != NULL);
    assert(state->knife_stroke != NULL);

    for (int i = 0; i < state->knife_stroke->size - 1; i++) {
        SDL_Point *p1 = array_get(state->knife_stroke, i);
        SDL_Point *p2 = array_get(state->knife_stroke, i + 1);

        for (int j = state->connections->size - 1; j >= 0; j--) {
            Connection *con = array_get(state->connections, j);

            Point a = {p1->x, p1->y};
            Point b = {p2->x, p2->y};
            Point c = {con->p1->parent_node->rect.x + con->p1->x, con->p1->parent_node->rect.y + con->p1->y};
            Point d = {con->p2->parent_node->rect.x + con->p2->x, con->p2->parent_node->rect.y + con->p2->y};
            Point out;

            if (segment_intersection(a, b, c, d, &out)) {
                array_remove_at(state->connections, j);
                free(con);
            }
        }
    }
}

void toggle_play_pause(SimulationState *state, void *function_data) {
    assert(state != NULL);
    (void)function_data;

    state->input.is_paused = !state->input.is_paused;
    printf("state->input.is_paused: %d\n", state->input.is_paused);
}

void one_step(SimulationState *state, void *function_data) {
    assert(state != NULL);
    (void)function_data;

    for (int i = 0; i < state->nodes->size; i++) {
        Node *node = array_get(state->nodes, i);
        // print_node(node);
        run_node(node);
    }

    for (int i = 0; i < state->connections->size; i++) {
        Connection *con = array_get(state->connections, i);
        set_input_zero(con);
    }

    for (int i = 0; i < state->connections->size; i++) {
        Connection *con = array_get(state->connections, i);
        propagate_state(con);
    }

    printf("\n\n\n");
}



void process_mouse_motion(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");
    assert(state->nodes != NULL && "Nodes array must be initialized");
    assert(state->buttons != NULL && "Buttons array must be initialized");

    float world_x, world_y;
    screen_to_world(state, state->input.mouse_x, state->input.mouse_y, &world_x, &world_y);

    // Knife-Stroke
    if (state->input.right_mouse_down) {
        Uint32 now = SDL_GetTicks();
        if (now - state->last_knife_record_time > 20) {
            SDL_Point *point = malloc(sizeof(SDL_Point));
            if (!point) {
                printf("Out of memory!\n");
                exit(EXIT_FAILURE);
            }
            point->x = world_x;
            point->y = world_y;
            array_add(state->knife_stroke, point);
            state->last_knife_record_time = now;
        }
    }

    // pin hover
    // todo: chunk based checks to not for loop over all items for mouse.
    state->hovered_pin = NULL;
    for (int i = 0; i < state->nodes->size; i++) {
        Node *node = array_get(state->nodes, i);

        for (int n = 0; n < node->inputs->size; n++) {
            Pin *pin = (Pin *)array_get(node->inputs, n);

            if (world_x >= pin->x + node->rect.x && world_x <= pin->x + node->rect.x + PIN_SIZE &&
                world_y >= pin->y + node->rect.y && world_y <= pin->y + node->rect.y + PIN_SIZE) {
                state->hovered_pin = pin;
                return;
            }
        }

        for (int n = 0; n < node->outputs->size; n++) {
            Pin *pin = (Pin *)array_get(node->outputs, n);
            if (world_x >= pin->x + node->rect.x && world_x <= pin->x + node->rect.x + PIN_SIZE &&
                world_y >= pin->y + node->rect.y && world_y <= pin->y + node->rect.y + PIN_SIZE) {
                state->hovered_pin = pin;
                return;
            }
        }
    }

    // move nodes
    if (state->input.is_node_dragging && state->dragged_node != NULL) {
        state->dragged_node->rect.x = world_x - state->input.drag_offset_x;
        state->dragged_node->rect.y = world_y - state->input.drag_offset_y;
    }
}

void process_left_click(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");
    assert(state->nodes != NULL && "Nodes array must be initialized");
    assert(state->buttons != NULL && "Buttons array must be initialized");

    if (state->input.left_mouse_down) {
        // in screen space
        for (int i = 0; i < state->buttons->size; i++) {
            Button *button = array_get(state->buttons, i);
            assert(button != NULL && "Button should not be NULL");
            if (state->input.mouse_x >= button->rect.x && state->input.mouse_x <= button->rect.x + button->rect.w &&
                state->input.mouse_y >= button->rect.y && state->input.mouse_y <= button->rect.y + button->rect.h) {
                button->on_press(state, button);
                return;
            }
        }

        // in World space
        float world_x, world_y;
        screen_to_world(state, state->input.mouse_x, state->input.mouse_y, &world_x, &world_y);

        for (int i = state->nodes->size - 1; i >= 0; i--) {
            Node *node = array_get(state->nodes, i);
            if (world_x >= node->rect.x && world_x <= node->rect.x + node->rect.w &&
                world_y >= node->rect.y && world_y <= node->rect.y + node->rect.h) {
                state->input.drag_offset_x = world_x - node->rect.x;
                state->input.drag_offset_y = world_y - node->rect.y;
                state->input.is_node_dragging = 1;
                state->dragged_node = node;
                break;
            }
        }

        if (state->hovered_pin != NULL) {
            if (state->first_selected_pin == NULL) {
                state->first_selected_pin = state->hovered_pin;
            }
            else if (state->first_selected_pin->is_input == state->hovered_pin->is_input) {
                state->first_selected_pin = NULL;
            }
            else {
                array_add(state->connections, create_connection(state->first_selected_pin, state->hovered_pin));
                state->first_selected_pin = NULL;
            }
        }
    }
}

void process_right_click(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");
    assert(state->nodes != NULL && "Nodes array must be initialized");
    
    if (state->input.right_mouse_down) {
        float world_x, world_y;
        screen_to_world(state, state->input.mouse_x, state->input.mouse_y, &world_x, &world_y);

        for (int i = 0; i < state->nodes->size; i++) {
            Node *node = array_get(state->nodes, i);

            if (strncmp(node->name, "SWITCH", 6) != 0)
                continue;

            if (world_x < node->rect.x || world_x > node->rect.x + node->rect.w ||
                world_y < node->rect.y || world_y > node->rect.y + node->rect.h)
                continue;

            for (int j = 0; j < node->outputs->size; j++) {
                Pin *p = array_get(node->outputs, j);
                p->state = !p->state;
            }
            break;
        }
    }
}

void process_right_mouse_up(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");
    connection_stroke_intersection(state);
    array_free_with_elements(state->knife_stroke);
    state->knife_stroke = array_create(16);
}

void process_left_mouse_up(SimulationState *state) {
    assert(state != NULL && "Cannot update NULL state");
    assert(state->buttons != NULL && "Buttons array must be initialized");

    // trash button area
    if (state->input.mouse_x >= WINDOW_WIDTH - 1 * (BUTTON_HEIGHT + 10) && state->input.mouse_x <= WINDOW_WIDTH - 1 * (BUTTON_HEIGHT + 10) + BUTTON_HEIGHT  &&
        state->input.mouse_y >= WINDOW_HEIGHT - BUTTON_HEIGHT - 10 && state->input.mouse_y <= WINDOW_HEIGHT - BUTTON_HEIGHT - 10 + BUTTON_HEIGHT) {
        
        for (int i = state->connections->size - 1; i >= 0; i--) {
            Connection *con = array_get(state->connections, i);
            if (con->p1->parent_node == state->last_dragged_node || con->p2->parent_node == state->last_dragged_node) {
                array_remove_at(state->connections, i);
                free(con);
            }
        }

        for (int i = 0; i < state->nodes->size; i++) {
            Node *node = array_get(state->nodes, i);
            if (node == state->last_dragged_node) {
                array_remove_at(state->nodes, i);
                free(node);
                state->last_dragged_node = NULL;
                break;
            }
        }
    }
}