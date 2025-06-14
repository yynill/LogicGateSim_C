#include "input_handler.h"

void handle_input(SimulationState *state, SDL_Event *event) {
    assert(state != NULL);
    assert(event != NULL);

    if (state->popup_state->is_pop) SDL_StartTextInput();
    else SDL_StopTextInput();

    switch(event->type) {
        case SDL_TEXTINPUT:
            if (state->popup_state->is_pop) {
                strncat(state->popup_state->name_input.text, event->text.text, 
                        sizeof(state->popup_state->name_input.text) - strlen(state->popup_state->name_input.text) - 1);
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            handle_mouse_button_down(state, event);
            break;

        case SDL_MOUSEBUTTONUP:
            handle_mouse_button_up(state, event);
            break;

        case SDL_MOUSEMOTION:
            handle_mouse_motion(state, event);
            break;

        case SDL_MOUSEWHEEL:
            handle_mouse_wheel(state, event);
            break;

        case SDL_KEYDOWN: {
            SDL_Keymod mod = SDL_GetModState();

            if ((mod & KMOD_GUI) && event->key.keysym.sym == SDLK_c) {
                handle_copy(state);
            }
            else if ((mod & KMOD_GUI) && event->key.keysym.sym == SDLK_v) {
                handle_paste(state);
            }
            else if ((mod & KMOD_GUI) && event->key.keysym.sym == SDLK_g) {
                handle_g_pressed(state);
            }
            else if (event->key.keysym.sym == SDLK_BACKSPACE) {
                handle_backspace(state);
            }
            else if (event->key.keysym.sym == SDLK_ESCAPE) {
                void *nothing = NULL;
                handle_escape(state, nothing);
            }
            else if (event->key.keysym.sym == SDLK_RETURN || event->key.keysym.sym == SDLK_KP_ENTER) {
                void *nothing = NULL;
                handle_enter(state, nothing);
            }
            break;
        }
        
        case SDL_KEYUP:
            break;

        case SDL_QUIT:
            state->is_running = 0;
            break;
    }
}

void handle_mouse_wheel(SimulationState *state, SDL_Event *event) {
    float old_zoom = state->camera_zoom;
    state->mouse_wheel = event->wheel.y;

    if (event->wheel.y > 0) {
        state->camera_zoom *= 1.1f;
    } 
    else if (event->wheel.y < 0) {
        state->camera_zoom /= 1.1f;
    }

    // Clamp zoom
    if (state->camera_zoom < 0.1f) state->camera_zoom = 0.1f;
    if (state->camera_zoom > 10.0f) state->camera_zoom = 10.0f;

    // zoom to center 
    float world_center_x = (WINDOW_WIDTH  / 2.0f) / old_zoom + state->camera_x;
    float world_center_y = (WINDOW_HEIGHT / 2.0f) / old_zoom + state->camera_y;

    state->camera_x = world_center_x - (WINDOW_WIDTH  / 2.0f) / state->camera_zoom;
    state->camera_y = world_center_y - (WINDOW_HEIGHT / 2.0f) / state->camera_zoom;
}

void handle_mouse_button_down(SimulationState *state, SDL_Event *event) {
    state->mouse_x = event->button.x;
    state->mouse_y = event->button.y;

    switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            state->left_mouse_down = 1;
            process_left_click(state);
            break;

        case SDL_BUTTON_RIGHT:
            state->right_mouse_down = 1;
            process_right_click(state);
            break;

        case SDL_BUTTON_MIDDLE:
            state->drag_offset_x = event->button.x;
            state->drag_offset_y = event->button.y;
            state->middle_mouse_down = 1;
            // check_middle_click(state); todo
            break;
    }
}


void handle_mouse_button_up(SimulationState *state, SDL_Event *event) {
    state->mouse_x = event->button.x;
    state->mouse_y = event->button.y;
    state->drag_offset_x = 0;
    state->drag_offset_y = 0;
    state->is_node_dragging = 0;
    state->is_camera_dragging = 0;

    switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            state->left_mouse_down = 0;
            state->last_dragged_node = state->dragged_node;
            state->dragged_node = NULL;
            process_left_mouse_up(state);
            break;

        case SDL_BUTTON_RIGHT:
            state->right_mouse_down = 0;
            process_right_mouse_up(state);
            break;

        case SDL_BUTTON_MIDDLE:
            state->middle_mouse_down = 0;
            // check_middle_mouse_up(state); todo
            break;
    }
}


void handle_mouse_motion(SimulationState *state, SDL_Event *event) {
    state->mouse_x = event->motion.x;
    state->mouse_y = event->motion.y;

    if (state->left_mouse_down) state->is_node_dragging = 1;
    if (state->middle_mouse_down) state->is_camera_dragging = 1;

    if (state->is_camera_dragging) {
        state->camera_x -= (event->motion.x - state->drag_offset_x) / state->camera_zoom;
        state->camera_y -= (event->motion.y - state->drag_offset_y) / state->camera_zoom;
        state->drag_offset_x = event->motion.x;
        state->drag_offset_y = event->motion.y;
    }

    process_mouse_motion(state);
}
