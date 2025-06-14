#ifndef POPUPSTATE_H
#define POPUPSTATE_H
#pragma once

#include <assert.h>
#include <SDL2/SDL.h>

#include "renderer.h"
#include "button.h"

typedef struct Button Button;

typedef struct {
    SDL_Rect rect;
    char text[256];
} TextInput;

typedef struct PopupState {
    int is_pop;
    SDL_Rect rect;

    TextInput name_input;

    DynamicArray *buttons;
} PopupState;


PopupState *init_popupstate();
int try_handle_popup(SimulationState *state);

#endif // POPUPSTATE_H
