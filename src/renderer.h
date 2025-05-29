#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "simulation.h"

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Texture *circle_texture;
} RenderContext;

RenderContext *init_renderer();
void clear_screen(RenderContext *context, SimulationState *sim_state);
void present_screen(RenderContext *context);
void cleanup_renderer(RenderContext *context);
void render_text(RenderContext *context, const char *text, int x, int y, SDL_Color color);
void render(RenderContext *context, SimulationState *sim_state);

#endif // RENDERER_H