#ifndef Button_H
#define Button_H

#include <SDL2/SDL.h>
#include "simulation.h"

typedef struct Button
{
    SDL_Rect rect;
    char *name;
    void *function_data;
    void (*on_press)(SimulationState *state, void *function_data); // Function pointer for button press action
} Button;

Button *create_button(SDL_Rect rect, char *name, void *function_data, void (*on_press)(SimulationState*, void*));

#endif // Button_H