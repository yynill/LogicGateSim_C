#include "renderer.h"
#include "../main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

RenderContext *init_renderer()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    if (TTF_Init() < 0)
    {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return NULL;
    }

    RenderContext *context = malloc(sizeof(RenderContext));
    if (!context)
    {
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    context->window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!context->window)
    {
        free(context);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    // Set texture scaling quality for better quality 0:nearest, 1:linear, 2:best
    // NOTE: I see no difference between 0, 1 and 2
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    context->renderer = SDL_CreateRenderer(context->window, -1, SDL_RENDERER_ACCELERATED);

    if (!context->renderer)
    {
        SDL_DestroyWindow(context->window);
        free(context);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    // Load Roboto font with higher resolution
    context->font = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 16);
    if (!context->font)
    {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(context->renderer);
        SDL_DestroyWindow(context->window);
        free(context);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    // Enable font hinting for better rendering
    TTF_SetFontHinting(context->font, TTF_HINTING_LIGHT);

    // Load circle texture
    // context->circle_texture = IMG_LoadTexture(context->renderer, "assets/images/circle.png");

    return context;
}

void clear_screen(RenderContext *context)
{
    assert(context != NULL);
    SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);
    SDL_RenderClear(context->renderer);
}

void present_screen(RenderContext *context)
{
    assert(context != NULL);
    SDL_RenderPresent(context->renderer);
}

void cleanup_renderer(RenderContext *context)
{
    assert(context != NULL);
    TTF_CloseFont(context->font);
    SDL_DestroyRenderer(context->renderer);
    SDL_DestroyWindow(context->window);
    SDL_DestroyTexture(context->circle_texture);
    free(context);
    TTF_Quit();
    SDL_Quit();
}

void render_text(RenderContext *context, const char *text, int x, int y, SDL_Color color)
{
    assert(context != NULL);
    assert(text != NULL);

    SDL_Surface *surface = TTF_RenderText_Solid(context->font, text, color);
    if (!surface)
    {
        printf("Failed to render text! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(context->renderer, surface);
    if (!texture)
    {
        printf("Failed to create texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(context->renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void render_top_bar(RenderContext *context)
{
    SDL_SetRenderDrawColor(context->renderer, 40, 40, 40, 255); // Dark gray background
    SDL_Rect top_bar = {0, 0, WINDOW_WIDTH, TOP_BAR_HEIGHT};
    SDL_RenderFillRect(context->renderer, &top_bar);
}

void render(RenderContext *context, SimulationState *sim_state)
{
    assert(context != NULL);
    assert(sim_state != NULL);

    clear_screen(context);
    render_top_bar(context);
    render_text(context, "Hello, World!", 10, 10, (SDL_Color){255, 255, 255, 255}); // White text on dark background
    present_screen(context);
}
