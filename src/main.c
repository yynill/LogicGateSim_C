#include "main.h"

#include "simulation.h"
#include "input_handler.h"
#include "renderer.h"
#include <stdio.h>

int main() {
    SimulationState *state = simulation_init();
    RenderContext *context = init_renderer();

    if (!state || !context) {
        printf("Failed to initialize simulation or renderer\n");
        return 1;
    }

    while (state->is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handle_input(state, &event);
        }

        if (state->should_reset) {
            simulation_cleanup(state);
            state = simulation_init();
            if (!state) {
                printf("Failed to reinitialize simulation\n");
                break;
            }
            continue;
        }

        simulation_update(state);
        render(context, state);
        SDL_Delay(FRAME_DELAY);
    }

    simulation_cleanup(state);
    cleanup_renderer(context);
    return 0;
}