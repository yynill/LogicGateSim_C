#include "simulation.h"
#include "renderer.h"
#include "main.h"
#include <stdio.h>

int main() {
    SimulationState *state = simulation_init();
    printf("Size of struct SimulationState: %zu bytes\n", sizeof(struct SimulationState));
    RenderContext *context = init_renderer();

    if (!state || !context) {
        printf("Failed to initialize simulation or renderer\n");
        return 1;
    }

    while (state->is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            simulation_handle_input(state, &event);
        }
        if (state->should_reset) {
            simulation_cleanup(state);
            state = simulation_init();
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