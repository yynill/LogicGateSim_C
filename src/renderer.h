#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "simulation.h"

#define TOP_BAR_HEIGHT 48

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT ((800) + TOP_BAR_HEIGHT) 

#define NODE_WIDTH 78
#define SMALL_NODE_WIDTH 54
#define NODE_HEIGHT 36
#define PIN_SIZE 10

#define BUTTON_WIDTH 48
#define BUTTON_HEIGHT 32


typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Texture *circle_texture;
} RenderContext;

RenderContext *init_renderer();
void clear_screen(RenderContext *context);
void present_screen(RenderContext *context);
void cleanup_renderer(RenderContext *context);
void render_text(RenderContext *context, const char *text, int x, int y, SDL_Color color);
void render_img(RenderContext *context, const char *path, SDL_Rect *rect) ;
void render(RenderContext *context, SimulationState *sim_state);

#endif // RENDERER_H