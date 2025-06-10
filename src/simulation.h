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

typedef struct SimulationState
{
    int is_running;
    int should_reset;

    DynamicArray *nodes;
    DynamicArray *connections;
    DynamicArray *buttons;

    Uint32 last_knife_record_time;
    DynamicArray *knife_stroke; // SDL_Point

    int left_mouse_down;
    int right_mouse_down;
    int middle_mouse_down;

    int mouse_x;
    int mouse_y;
    int mouse_wheel;
    int drag_offset_x;
    int drag_offset_y;

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

    SDL_Point *hovered_connection_point;
    SDL_Point *dragging_connection_point;
    Connection *new_connection;

    Pin *hovered_pin;
    Pin *first_selected_pin;
} SimulationState;

// Function declarations
SimulationState *simulation_init(void);
void simulation_cleanup(SimulationState *state);
void simulation_update(SimulationState *state);
void reset_sim(SimulationState *state, void *function_data);

void null_function(SimulationState *state, void *function_data);
void add_node(SimulationState *state, void *function_data);
void connection_stroke_intersection(SimulationState *state);
void one_step(SimulationState *state, void *function_data);
void toggle_play_pause(SimulationState *state, void *function_data);

void process_left_click(SimulationState *state);
void process_right_click(SimulationState *state);
void process_mouse_motion(SimulationState *state);
void process_left_mouse_up(SimulationState *state);
void process_right_mouse_up(SimulationState *state);


#endif // SIMULATION_H
