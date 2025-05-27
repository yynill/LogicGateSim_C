#ifndef MAIN_H
#define MAIN_H

// Window configuration
#define WINDOW_TITLE "Logic Gate Sim"

#define TOP_BAR_HEIGHT 50
#define SCALE_FACTOR 5

#define WINDOW_WIDTH (160 * SCALE_FACTOR)
#define WINDOW_HEIGHT (144 * SCALE_FACTOR) + TOP_BAR_HEIGHT

// Timing configuration
#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS) // Delay in milliseconds

// Input configuration
#define KEY_UP SDLK_UP
#define KEY_DOWN SDLK_DOWN
#define KEY_LEFT SDLK_LEFT
#define KEY_RIGHT SDLK_RIGHT
#define KEY_A SDLK_z
#define KEY_B SDLK_x
#define KEY_START SDLK_RETURN
#define KEY_SELECT SDLK_SPACE

#endif // MAIN_H