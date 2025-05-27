#include "renderer.h"
#include "main.h"
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

    return context;
}

void clear_screen(RenderContext *context)
{
    assert(context != NULL);
    SDL_SetRenderDrawColor(context->renderer, 50, 50, 50, 255);
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

void render_img(RenderContext *context, const char *path, SDL_Rect *rect)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        printf("Failed to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(context->renderer, surface);
    if (!texture)
    {
        printf("Failed to create texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_RenderCopy(context->renderer, texture, NULL, rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void render_top_bar(RenderContext *context)
{
    SDL_SetRenderDrawColor(context->renderer, 40, 40, 40, 255); // Dark gray background
    SDL_Rect top_bar = {0, 0, WINDOW_WIDTH, TOP_BAR_HEIGHT};
    SDL_RenderFillRect(context->renderer, &top_bar);
}

void render_button(RenderContext *context, Button *button)
{
    // Draw button hit box
    SDL_SetRenderDrawColor(context->renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(context->renderer, &button->rect);

    render_img(context, button->path, &button->rect);
}

void render_node(RenderContext *context, Node *node)
{
    SDL_Rect node_rect = node->rect;

    // Draw node background
    SDL_SetRenderDrawColor(context->renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(context->renderer, &node_rect);

    // Draw node border
    SDL_SetRenderDrawColor(context->renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(context->renderer, &node_rect);

    render_img(context, node->path, &node_rect);
}

void render(RenderContext *context, SimulationState *sim_state)
{
    assert(context != NULL);
    assert(sim_state != NULL);

    clear_screen(context);
    render_top_bar(context);

    // Render all buttons
    for (int i = 0; i < sim_state->buttons->size; i++)
    {
        Button *button = array_get(sim_state->buttons, i);
        render_button(context, button);
    }

    // Render all nodes
    for (int i = 0; i < sim_state->nodes->size; i++)
    {
        Node *node = array_get(sim_state->nodes, i);
        render_node(context, node);
    }

    present_screen(context);
}
