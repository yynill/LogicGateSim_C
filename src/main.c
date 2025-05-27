#include "simulation.h"
#include "renderer.h"
#include "main.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    (void)argc; // Silence unused parameter warning
    (void)argv; // Silence unused parameter warning

    SimulationState *state = simulation_init();
    RenderContext *context = init_renderer();

    if (!state || !context)
    {
        printf("Failed to initialize simulation or renderer\n");
        return 1;
    }

    // Main game loop
    while (state->is_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            simulation_handle_input(state, &event);
        }

        simulation_update(state);
        render(context, state); // Add rendering call
        SDL_Delay(FRAME_DELAY); // Cap at TARGET_FPS
    }

    simulation_cleanup(state);
    cleanup_renderer(context); // Add renderer cleanup
    return 0;
}