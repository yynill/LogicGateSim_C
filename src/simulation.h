#ifndef SIMULATION_H
#define SIMULATION_H

#include <SDL2/SDL.h>
#include "DynamicArray.h"
#include "node.h"

typedef struct SimulationState
{
    int is_running;
    int should_reset;

    DynamicArray *nodes;
    DynamicArray *connections;
    DynamicArray *buttons;

    Uint32 last_knife_record_time;
    DynamicArray *knife_stroke; // SDL_Point

    struct
    {
        int left_mouse_down;
        int right_mouse_down;
        int mouse_up;
        int mouse_x;
        int mouse_y;

        int is_dragging;
        int drag_offset_x;
        int drag_offset_y;
        int is_paused;
    } input;
    Node *dragged_node;
    Node *last_dragged_node;

    Pin *hovered_pin;
    // todo add last selcted pin or drag system
    Pin *first_selected_pin;
} SimulationState;

typedef struct Button
{
    SDL_Rect rect;
    char *name;
    void *function_data;
    void (*on_press)(SimulationState *state, void *function_data); // Function pointer for button press action
} Button;

// Function declarations
SimulationState *simulation_init(void);
void simulation_cleanup(SimulationState *state);
void simulation_update(SimulationState *state);
void simulation_handle_input(SimulationState *state, SDL_Event *event);
void add_node(SimulationState *state, void *function_data);
void check_left_click(SimulationState *state);
void check_right_click(SimulationState *state);
void check_motion_pos(SimulationState *state);
void check_left_mouse_up(SimulationState *state);
void check_right_mouse_up(SimulationState *state);
void one_step(SimulationState *state, void *function_data);
void toggle_play_pause(SimulationState *state, void *function_data);
void reset_sim(SimulationState *state, void *function_data);

#endif // SIMULATION_H
