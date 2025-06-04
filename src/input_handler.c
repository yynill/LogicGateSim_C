#include "input_handler.h"

void handle_input(SimulationState *state, SDL_Event *event) {
    assert(state != NULL);
    assert(event != NULL);

    switch(event->type) {
        case SDL_MOUSEBUTTONDOWN:
            handle_mouse_button_down(state, event);
            break;

        case SDL_MOUSEBUTTONUP:
            handle_mouse_button_up(state, event);
            break;

        case SDL_MOUSEMOTION:
            handle_mouse_motion(state, event);
            break;

        case SDL_KEYDOWN:
            break;

        case SDL_KEYUP:
            break;

        case SDL_QUIT:
            state->is_running = 0;
            break;
    }
}

void handle_mouse_button_down(SimulationState *state, SDL_Event *event){
    switch (event->button.button) {
        
        state->input.mouse_x = event->button.x;
        state->input.mouse_y = event->button.y;

        case SDL_BUTTON_LEFT:
            state->input.left_mouse_down = 1;
            process_left_click(state);
            break;

        case SDL_BUTTON_RIGHT:
            state->input.right_mouse_down = 1;
            process_right_click(state);
            break;

        case SDL_BUTTON_MIDDLE:
            state->input.middle_mouse_down = 1;
            // check_middle_click(state); todo
            break;
    }
}

void handle_mouse_button_up(SimulationState *state, SDL_Event *event) {
    switch (event->button.button) {

        state->input.mouse_x = event->button.x;
        state->input.mouse_y = event->button.y;

        state->input.is_node_dragging = 0;
        state->input.is_camera_dragging = 0;

        case SDL_BUTTON_LEFT:
            state->input.left_mouse_down = 0;
            state->last_dragged_node = state->dragged_node;
            state->dragged_node = NULL;
            process_left_mouse_up(state);
            break;

        case SDL_BUTTON_RIGHT:
            state->input.right_mouse_down = 0;
            process_right_mouse_up(state);
            break;

        case SDL_BUTTON_MIDDLE:
            state->input.middle_mouse_down = 0;
            // check_middle_mouse_up(state); todo
            break;
    }
}

void handle_mouse_motion(SimulationState *state, SDL_Event *event) {
    state->input.mouse_x = event->motion.x;
    state->input.mouse_y = event->motion.y;

    if (state->input.left_mouse_down) state->input.is_node_dragging = 1;
    if (state->input.middle_mouse_down) state->input.is_camera_dragging = 1;

    process_mouse_motion(state);
}
