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

    TTF_SetFontHinting(context->font, TTF_HINTING_LIGHT);

    context->circle_texture = IMG_LoadTexture(context->renderer, "assets/images/circle.png");
    if (!context->circle_texture)
    {
        printf("Failed to load circle texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(context->renderer);
        SDL_DestroyWindow(context->window);
        free(context);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    assert(context->window != NULL && "Window must be created");
    assert(context->renderer != NULL && "Renderer must be created");
    assert(context->font != NULL && "Font must be loaded");

    return context;
}

void clear_screen(RenderContext *context)
{
    assert(context != NULL);
    SDL_SetRenderDrawColor(context->renderer, 100, 100, 100, 255);
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

void screen_to_world(SimulationState *state, int screen_x, int screen_y, float *world_x, float *world_y) {
    *world_x = (screen_x / state->input.camera_zoom) + state->input.camera_x;
    *world_y = (screen_y / state->input.camera_zoom) + state->input.camera_y;
}

void world_to_screen(SimulationState *state, float world_x, float world_y, int *screen_x, int *screen_y) {
    *screen_x = (int)((world_x - state->input.camera_x) * state->input.camera_zoom);
    *screen_y = (int)((world_y - state->input.camera_y) * state->input.camera_zoom);
}


void render_text(RenderContext *context, const char *text, int x, int y, SDL_Color color, float zoom) {
    assert(context != NULL);
    assert(text != NULL);
    assert(context->font != NULL && "Font must be loaded for text rendering");

    if (zoom < 0.8) return;
    

    SDL_Surface *surface = TTF_RenderText_Solid(context->font, text, color);
    if (!surface) {
        printf("Failed to render text! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(context->renderer, surface);
    if (!texture) {
        printf("Failed to create texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {
        x,
        y,
        (int)(surface->w * zoom),
        (int)(surface->h * zoom)};

    SDL_RenderCopy(context->renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void render_img(RenderContext *context, const char *path, SDL_Rect *rect) {
    assert(context != NULL);
    assert(path != NULL);
    assert(rect != NULL);
    assert(rect->w > 0 && rect->h > 0 && "Image dimensions must be positive");

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

void render_top_bar(RenderContext *context) {
    assert(context != NULL);
    SDL_SetRenderDrawColor(context->renderer, 40, 40, 40, 255);
    SDL_Rect top_bar = {0, 0, WINDOW_WIDTH, TOP_BAR_HEIGHT};
    SDL_RenderFillRect(context->renderer, &top_bar);
}

void render_button(RenderContext *context, Button *button) {
    assert(context != NULL);
    assert(button != NULL);
    assert(button->name != NULL && "Button must have a valid image path");
    assert(button->rect.w > 0 && button->rect.h > 0 && "Button dimensions must be positive");

    int text_width, text_height;
    if (strncmp(button->name, "/", 1) == 0) {
        render_img(context, button->name, &button->rect);
    }
    else {
        SDL_SetRenderDrawColor(context->renderer, 20, 20, 20, 255);
        SDL_RenderFillRect(context->renderer, &button->rect);

        SDL_Color text_color = {255, 255, 255, 255};
        if (TTF_SizeText(context->font, button->name, &text_width, &text_height) == 0) {
            int text_x = button->rect.x + (button->rect.w - text_width) / 2;
            int text_y = button->rect.y + (button->rect.h - text_height) / 2;

            render_text(context, button->name, text_x, text_y, text_color, 1);
        }
        else {
            render_text(context, button->name, button->rect.x, button->rect.y, text_color, 1);
        }
    }
}

void render_knife_stroke(RenderContext *context, SimulationState *sim_state) {
    assert(context != NULL);
    assert(sim_state != NULL);
    SDL_SetRenderDrawColor(context->renderer, 255, 255, 255, 255);

    for (int i = 0; i < sim_state->knife_stroke->size - 1; i++) {
        SDL_Point *p1 = array_get(sim_state->knife_stroke, i);
        SDL_Point *p2 = array_get(sim_state->knife_stroke, i + 1);

        int x1, y1, x2, y2;
        world_to_screen(sim_state, (float)p1->x, (float)p1->y, &x1, &y1);
        world_to_screen(sim_state, (float)p2->x, (float)p2->y, &x2, &y2);

        SDL_RenderDrawLine(context->renderer, x1, y1, x2, y2);
    }
}

void render_node(RenderContext *context, Node *node, SimulationState *sim_state) {
    assert(context != NULL);
    assert(node != NULL);
    assert(sim_state != NULL);

    SDL_Rect node_rect = node->rect;
    if (sim_state->dragged_node != NULL && sim_state->dragged_node == node) {
        node_rect.x -= 2;
        node_rect.y -= 2;
        node_rect.w += 4;
        node_rect.h += 4;
        // todo: move each pin for drag zooom 
    }

    printf("screen: %d, %d, %d, %d\n", node_rect.x, node_rect.y, node_rect.w, node_rect.h);
    
    int new_x, new_y;
    world_to_screen(sim_state, node_rect.x, node_rect.y, &new_x, &new_y);
    int new_w = (int)(node_rect.w * sim_state->input.camera_zoom);
    int new_h = (int)(node_rect.h * sim_state->input.camera_zoom);
    SDL_Rect dest = {new_x, new_y, new_w, new_h};
    
    
    printf("world:  %d, %d, %d, %d\n", new_x, new_y, new_w, new_h);

    if (strcmp(node->name, "SWITCH") == 0) {
        Pin *p = array_get(node->outputs, 0);
        if (p->state) render_img(context, "/assets/images/switch_on.png", &dest);
        else render_img(context, "/assets/images/switch_off.png", &dest);
    } else if (strcmp(node->name, "LIGHT") == 0) {
        Pin *p = array_get(node->inputs, 0);
        if (p->state) render_img(context, "/assets/images/light_on.png", &dest);
        else render_img(context, "/assets/images/light_off.png", &dest);
    } else {
        if (strcmp(node->name, "AND") == 0) SDL_SetRenderDrawColor(context->renderer, 0, 128, 0, 255);
        else if (strcmp(node->name, "OR") == 0) SDL_SetRenderDrawColor(context->renderer, 255, 165, 0, 255);
        else if (strcmp(node->name, "NOT") == 0) SDL_SetRenderDrawColor(context->renderer, 128, 0, 0, 255);
        else if (strcmp(node->name, "NOR") == 0) SDL_SetRenderDrawColor(context->renderer, 128, 0, 128, 255);
        else if (strcmp(node->name, "NAND") == 0) SDL_SetRenderDrawColor(context->renderer, 0, 0, 128, 255);
        else if (strcmp(node->name, "XOR") == 0) SDL_SetRenderDrawColor(context->renderer, 255, 20, 147, 255);
        else if (strcmp(node->name, "XNOR") == 0) SDL_SetRenderDrawColor(context->renderer, 0, 139, 139, 255);
        else  SDL_SetRenderDrawColor(context->renderer, 0, 0, 255, 255);
        
        SDL_RenderFillRect(context->renderer, &dest);
    }

    SDL_SetTextureAlphaMod(context->circle_texture, 255);

    int num_inputs = node->inputs->size;
    for (int i = 0; i < num_inputs; i++) {
        Pin *pin = array_get(node->inputs, i);

        SDL_Rect pin_rect;
        world_to_screen(sim_state, node_rect.x + pin->x, node_rect.y + pin->y, &pin_rect.x, &pin_rect.y);
        pin_rect.w = (int)(PIN_SIZE * sim_state->input.camera_zoom);
        pin_rect.h = (int)(PIN_SIZE * sim_state->input.camera_zoom);

        if (sim_state->hovered_pin == pin) SDL_SetTextureColorMod(context->circle_texture, 255, 60, 60);
        else if (sim_state->first_selected_pin == pin) SDL_SetTextureColorMod(context->circle_texture, 0, 128, 255);
        else if (pin->state) SDL_SetTextureColorMod(context->circle_texture, 0, 200, 103);
        else SDL_SetTextureColorMod(context->circle_texture, 0, 0, 0);
        SDL_RenderCopy(context->renderer, context->circle_texture, NULL, &pin_rect);
    }

    int num_outputs = node->outputs->size;
    for (int i = 0; i < num_outputs; i++) {
        Pin *pin = array_get(node->outputs, i);

        SDL_Rect pin_rect;
        world_to_screen(sim_state, node_rect.x + pin->x, node_rect.y + pin->y, &pin_rect.x, &pin_rect.y);
        pin_rect.w = (int)(PIN_SIZE * sim_state->input.camera_zoom);
        pin_rect.h = (int)(PIN_SIZE * sim_state->input.camera_zoom);

        if (sim_state->hovered_pin == pin) SDL_SetTextureColorMod(context->circle_texture, 255, 60, 60);
        else if (sim_state->first_selected_pin == pin) SDL_SetTextureColorMod(context->circle_texture, 0, 128, 255);
        else if (pin->state) SDL_SetTextureColorMod(context->circle_texture, 0, 200, 103);
        else SDL_SetTextureColorMod(context->circle_texture, 0, 0, 0);  
        SDL_RenderCopy(context->renderer, context->circle_texture, NULL, &pin_rect);
    }

    if (strcmp(node->name, "SWITCH") != 0 && strcmp(node->name, "LIGHT") != 0) {
        SDL_Color text_color = {255, 255, 255, 255};
        int text_width, text_height;
        if (TTF_SizeText(context->font, node->name, &text_width, &text_height) == 0) {
            int text_x, text_y;
            world_to_screen(sim_state, node->rect.x, node->rect.y, &text_x, &text_y);
            text_x += (int)(((node->rect.w - text_width) * sim_state->input.camera_zoom) / 2);
            text_y += (int)(((node->rect.h - text_height) * sim_state->input.camera_zoom) / 2);
            render_text(context, node->name, text_x, text_y, text_color, sim_state->input.camera_zoom);
        } else {
            render_text(context, node->name, node->rect.x, node->rect.y, text_color, sim_state->input.camera_zoom);
        }
    }
}

void render_connection(RenderContext *context, Connection *con, SimulationState *sim_state) {
    assert(context != NULL);
    assert(con != NULL);

    Node *parent1 = con->p1->parent_node;
    Node *parent2 = con->p2->parent_node;

    int x1 = (int)((parent1->rect.x + con->p1->x) - sim_state->input.camera_x) * sim_state->input.camera_zoom + (PIN_SIZE * sim_state->input.camera_zoom / 2);
    int y1 = (int)((parent1->rect.y + con->p1->y) - sim_state->input.camera_y) * sim_state->input.camera_zoom + (PIN_SIZE * sim_state->input.camera_zoom / 2);
    int x2 = (int)((parent2->rect.x + con->p2->x) - sim_state->input.camera_x) * sim_state->input.camera_zoom + (PIN_SIZE * sim_state->input.camera_zoom / 2);
    int y2 = (int)((parent2->rect.y + con->p2->y) - sim_state->input.camera_y) * sim_state->input.camera_zoom + (PIN_SIZE * sim_state->input.camera_zoom / 2);

    if (con->state) {SDL_SetRenderDrawColor(context->renderer, 0, 200, 103, 255);}
    else {SDL_SetRenderDrawColor(context->renderer, 0, 0, 30, 255);}

    int thickness = (int)4 * sim_state->input.camera_zoom;
    for (int i = -thickness/2; i <= thickness/2; i++) {
        SDL_RenderDrawLine(context->renderer, x1, y1 + i, x2, y2 + i);
        SDL_RenderDrawLine(context->renderer, x1 + i, y1, x2 + i, y2);
    }
}


void render(RenderContext *context, SimulationState *sim_state)
{
    assert(context != NULL);
    assert(sim_state != NULL);
    assert(sim_state->buttons != NULL && "Buttons array must be initialized");
    assert(sim_state->nodes != NULL && "Nodes array must be initialized");

    clear_screen(context);

    render_knife_stroke(context, sim_state);

    for (int i = 0; i < sim_state->connections->size; i++) {
        Connection *connection = array_get(sim_state->connections, i);
        assert(connection != NULL && "Connection must not be NULL");

        render_connection(context, connection, sim_state);
    }

    Node *last_node = NULL;

    for (int i = 0; i < sim_state->nodes->size; i++) {
        Node *node = array_get(sim_state->nodes, i);
        assert(node != NULL && "Node must not be NULL");

        if (node == sim_state->dragged_node) {
            last_node = node;
            continue;
        }
        render_node(context, node, sim_state);
    }
    if (last_node != NULL) render_node(context, last_node, sim_state);

    render_top_bar(context);
    for (int i = 0; i < sim_state->buttons->size; i++) {
        Button *button = array_get(sim_state->buttons, i);

        if (strncmp(button->name, "/assets/images/play.png", 23) == 0 && !sim_state->input.is_paused) { button->name = "/assets/images/pause.png"; }
        if (strncmp(button->name, "/assets/images/pause.png", 24) == 0 && sim_state->input.is_paused) { button->name = "/assets/images/play.png"; }
        if (strncmp(button->name, "/assets/images/trash.png", 24) == 0 && !sim_state->dragged_node) { continue; }

        assert(button != NULL && "Button must not be NULL");
        render_button(context, button);
    }
    
    present_screen(context);
}
