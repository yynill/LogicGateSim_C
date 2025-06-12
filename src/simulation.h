#ifndef SIMULATION_H
#define SIMULATION_H
#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h> 

#include "main.h"
#include "renderer.h"
#include "DataStructures/DynamicArray.h"
#include "node.h"
#include "point.h"
#include "button.h"
#include "connection.h"

typedef struct Connection Connection;
typedef struct Button Button;

typedef struct SimulationState
{
    int is_running;
    int should_reset;

    DynamicArray *nodes;
    DynamicArray *connections;
    DynamicArray *buttons;

    Uint32 last_knife_record_time;
    DynamicArray *knife_stroke; // SDL_Point

    DynamicArray *selected_nodes;
    DynamicArray *selected_connection_points;

    int left_mouse_down;
    int right_mouse_down;
    int middle_mouse_down;

    int mouse_x;
    int mouse_y;
    int mouse_wheel;

    // relative position between where you clicked and the objects origin
    int drag_offset_x;
    int drag_offset_y;

    int is_selection_box_drawing;
    SDL_Rect selection_box;

    int is_node_dragging;
    int is_camera_dragging;
    int is_cable_dragging;
    int is_connection_point_dragging;

    int camera_x;
    int camera_y;
    float camera_zoom;

    int is_paused;

    Node *dragged_node;
    Node *last_dragged_node;
    int last_node_x;
    int last_node_y;

    SDL_Point *hovered_connection_point;
    SDL_Point *dragging_connection_point;
    Connection *new_connection;

    Pin *hovered_pin;
    Pin *first_selected_pin;
} SimulationState;

// Function declarations
SimulationState *simulation_init(void);
void init_buttons(SimulationState *state);
void simulation_cleanup(SimulationState *state);
void simulation_update(SimulationState *state);
void reset_sim(SimulationState *state, void *function_data);

void process_left_click(SimulationState *state);
void process_left_mouse_up(SimulationState *state);
void process_right_mouse_up(SimulationState *state);
void process_right_click(SimulationState *state);
void process_mouse_motion(SimulationState *state);

void null_function(SimulationState *state, void *function_data);
void add_node(SimulationState *state, void *function_data);
void cut_connection(SimulationState *state);
void one_step(SimulationState *state, void *function_data);
void toggle_play_pause(SimulationState *state, void *function_data);
void reset_interaction_state(SimulationState *state);
void reset_knife_stroke(SimulationState *state);
void start_selection_box(SimulationState *state);
void update_all_connections(SimulationState *state);
void handle_selection_box(SimulationState *state);
void handle_cable_dragging(SimulationState *state);
void cancel_cable_dragging(SimulationState *state);
void handle_node_dragging(SimulationState *state, float world_x, float world_y);
void handle_connection_point(SimulationState *state, float world_x, float world_y);
void update_pin_hover(SimulationState *state, float world_x, float world_y);
void handle_knife_stroke_motion(SimulationState *state, float world_x, float world_y);
int try_handle_node_click(SimulationState *state, float world_x, float world_y);
int try_handle_connection_point_click(SimulationState *state);
int try_handle_button_click(SimulationState *state);
int try_handle_pin_click(SimulationState *state);
int try_handle_trash_drop(SimulationState *state);
int try_complete_pin_connection(SimulationState *state);
void delete_node_and_connections(SimulationState *state, Node *node);
Button *find_button_at_position(SimulationState *state, int screen_x, int screen_y);
Node *find_node_at_position(SimulationState *state, float x, float y);
void start_selection_box(SimulationState *state);



#endif // SIMULATION_H
