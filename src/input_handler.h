#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H
#pragma once

#include <SDL2/SDL.h>

#include "simulation.h"
#include "main.h"
#include "renderer.h"

#include <assert.h>

void handle_input(SimulationState *state, SDL_Event *event);

void handle_mouse_button_down(SimulationState *state, SDL_Event *event);
void handle_mouse_button_up(SimulationState *state, SDL_Event *event);
void handle_mouse_motion(SimulationState *state, SDL_Event *event);
void handle_mouse_wheel(SimulationState *state, SDL_Event *event);

#endif
