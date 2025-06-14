# Compiler and flags configuration
CC = gcc
# Compiler flags: Enable warnings and debugging, include SDL2 and SDL2_ttf headers
CFLAGS = -Wall -Wextra -g \
-I/opt/homebrew/opt/sdl2/include \
-I/opt/homebrew/opt/sdl2/include/SDL2 \
-I/opt/homebrew/opt/sdl2_ttf/include \
-I/opt/homebrew/opt/sdl2_ttf/include/SDL2 \
-I/opt/homebrew/opt/sdl2_image/include \
-I/opt/homebrew/opt/sdl2_image/include/SDL2 \
-I/opt/homebrew/opt/sdl2_gfx/include \
-I/opt/homebrew/opt/sdl2_gfx/include/SDL2 \
-I/opt/homebrew/include/SDL2

# Linker flags: Link against SDL2 and SDL2_ttf libraries
LDFLAGS = -L/opt/homebrew/opt/sdl2/lib -L/opt/homebrew/opt/sdl2_ttf/lib -L/opt/homebrew/opt/sdl2_image/lib -L/opt/homebrew/opt/sdl2_gfx/lib -lSDL2_ttf -lSDL2 -lSDL2_image -lSDL2_gfx

# Main program configuration
MAIN_SRC = src/main.c \
		   src/DataStructures/DynamicArray.c \
           src/simulation.c \
           src/input_handler.c \
           src/renderer.c \
		   src/node.c \
		   src/node_group.c \
		   src/connection.c \
		   src/popup_state.c \
		   src/button.c \
		   src/point.c

MAIN_OBJ = $(MAIN_SRC:.c=.o)
MAIN_TARGET = main

# Declare phony targets (targets that don't create files)
.PHONY: all clean clear

# Default target: clear screen and build main program
all: clear $(MAIN_TARGET)

# Link main program objects into executable
$(MAIN_TARGET): $(MAIN_OBJ)
	$(CC) $(MAIN_OBJ) -o $@ $(LDFLAGS)

# Pattern rule for compiling C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(MAIN_OBJ) $(MAIN_TARGET) 