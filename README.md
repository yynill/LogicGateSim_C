# Logic Gate Simulator

A [logic gate](https://en.wikipedia.org/wiki/Logic_gate) simulator written in C, using SDL2 for visualization.
For development notes, known bugs, planned features, see `notes.txt`.
 
 <img width="1204" alt="Screenshot 2025-06-15 at 03 35 17" src="https://github.com/user-attachments/assets/71c23cb6-e667-4717-ad85-295ad40463e9" />


## Available Logic Gates
NOT, AND, OR, XOR, XNOR, NOR, NAND, Light (output indicator), Switch (input control)

## Controls

- **Mouse Controls**
  - Left Click + Drag: Move gates or create connections
  - Right Click + Drag: Cut wires
  - Right Click: Toggle switches
  - Left Click + Drag: Select multiple components

- **Keyboard Shortcuts**
  - `Backspace`: Delete selected components
  - `Cmd+C`: Copy selected components
  - `Cmd+V`: Paste copied components
  - `Cmd+G`: Create component group from selected
  - `Enter`: Confirm group creation
  - `Escape`: Cancel group creation

- **Simulation Controls** (Top Right Corner)
  - Play/Pause Button: Toggle automatic simulation
  - Step Button: Advance simulation by one step
  - Backtep Button: todo
  - Reset Button: Reset simulation

## Installation
### Installing Dependencies

#### macOS
```bash
brew install sdl2 sdl2_ttf sdl2_image sdl2_gfx
```

### Build Instructions
1. Clone the repository:
2. Build the project:
   ```bash
   make
   ```
3. Run the simulator:
   ```bash
   ./main
   ```


