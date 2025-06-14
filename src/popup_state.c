

#include "popup_state.h"

PopupState *init_popupstate() {
    PopupState *pop_state = malloc(sizeof(PopupState));
    if (!pop_state) return NULL;

    pop_state->is_pop = 0;

    int popup_w = 600;
    int popup_h = 80;

    int popup_x = (WINDOW_WIDTH - popup_w) / 2;
    int popup_y = (WINDOW_HEIGHT - popup_h) / 2;

    pop_state->rect.x = popup_x,
    pop_state->rect.y = popup_y,
    pop_state->rect.w = popup_w;
    pop_state->rect.h = popup_h;

    pop_state->name_input.rect.x = popup_x + 20;
    pop_state->name_input.rect.y = popup_y + 20;
    pop_state->name_input.rect.w = 300;
    pop_state->name_input.rect.h = 30;
    pop_state->name_input.text[0] = '\0';

    pop_state->buttons = array_create(16);
    assert(pop_state->buttons != NULL);

    int button_w = 40;
    int button_h = 20;
    int padding = 8;

    SDL_Rect enter_rect = {
        pop_state->rect.x + pop_state->rect.w - button_w - padding,
        pop_state->rect.y + pop_state->rect.h - button_h - padding,
        button_w,
        button_h
    };
    Button *enter_btn =  create_button(enter_rect, "enter", nullGate, handle_enter);
    array_add(pop_state->buttons, enter_btn);

    SDL_Rect esc_rect = {
        enter_rect.x - button_w - padding,
        enter_rect.y,
        button_w,
        button_h
    };
    Button *esc_btn =   create_button(esc_rect, "esc", nullGate, handle_escape);
    array_add(pop_state->buttons, esc_btn);

    return pop_state;
}

int try_handle_popup(SimulationState *state) {
    assert(state != NULL);
    Button *clicked_button = find_button_at_position(state->popup_state->buttons, state->mouse_x, state->mouse_y);
    if (clicked_button) {
        clicked_button->on_press(state, clicked_button);
        return 1;
    }
    return 0;
}