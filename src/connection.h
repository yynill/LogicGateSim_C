#ifndef CONNECTION_H
#define CONNECTION_H
#pragma once


#include <SDL2/SDL.h>
#include <assert.h>
#include "DataStructures/DynamicArray.h"
#include "node.h"
#include "renderer.h"

typedef struct Connection
{
    Pin *p1;
    Pin *p2;
    int state;
    DynamicArray *points; // SDL_Points
} Connection;

Connection *start_connection(Pin *pin1);
void add_connection_point(Connection *con, int x, int y);
void pop_connection_point(Connection *con);
void finish_conection(Connection *con, Pin *pin2);

void propagate_state(Connection *con);
void set_input_zero(Connection *con);
void correct_connection_points(Connection *con);
void update_connection_points(SimulationState *state, Connection *con);

void print_connection(Connection *con);
void connection_free(Connection *con);

#endif // CONNECTION_H
