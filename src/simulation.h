#ifndef SIMULATION_H
#define SIMULATION_H

#include <SDL2/SDL.h>
#include "DynamicArray.h"
#include "node.h"

typedef struct SimulationState
{
    int is_running;
    DynamicArray *nodes;
    DynamicArray *connections;
    DynamicArray *buttons;

    struct
    {
        int mouse_down;
        int mouse_up;
        int mouse_x;
        int mouse_y;

        int is_dragging;
        int drag_offset_x;
        int drag_offset_y;
    } input;    
    Node *dragged_node;
    
    int connection_mode;
    Node *first_connection_node;
} SimulationState;    

typedef struct Button
{
    SDL_Rect rect;
    const char *name;
    void *function_data;
    void (*on_press)(SimulationState *state, void *function_data); // Function pointer for button press action
} Button;    

// Function declarations
SimulationState *simulation_init(void);
void simulation_cleanup(SimulationState *state);
void simulation_update(SimulationState *state);
void simulation_handle_input(SimulationState *state, SDL_Event *event);
void add_node(SimulationState *state, void *function_data);
void check_click_pos(SimulationState *state);
void connection_mode(SimulationState *state, void *function_data);

#endif // SIMULATION_H
