```markdown
# Dynamic Pong

A classic Pong game implementation in C++ using SDL2, featuring dynamic ball colors and particle effects on paddle collision.

## Description of the project

This project is a rendition of the classic Pong game, developed in C++ using the SDL2 library. It enhances the traditional gameplay with dynamic elements such as a color-shifting ball and particle effects that are triggered upon collision with paddles, adding a visually engaging twist to the retro arcade experience.

## Getting Started

### Prerequisites
Ensure you have the following installed:
- C++ Compiler (e.g., g++)
- [SDL2](https://www.libsdl.org/) (version 2.0 or higher)
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/) (SDL_ttf version 2.0.14 or higher)

**Installation of SDL2 and SDL2_ttf:**

* **Ubuntu/Debian:**
  ```bash
  sudo apt-get update
  sudo apt-get install libsdl2-dev libsdl2-ttf-dev
  ```
* **macOS (using Homebrew):**
  ```bash
  brew install sdl2 sdl2_ttf
  ```
* **Windows (using vcpkg):**
  ```bash
  vcpkg install sdl2 sdl2-ttf
  ```
  Follow vcpkg's instructions to integrate it with your build system or set environment variables appropriately.

## Project Structure

```
Colorfull-Ball-with-Particle-Effect-Animation/
├── pong.cc    # Main C++ source file
├── Makefile             # Build script for the project
└── README.md            # Project documentation (this file)
```

### Installation
1. Clone the repository:
   ```bash
   git clone git@github.com:Luke23-45/Colorfull-Ball-with-Particle-Effect-Animation.git
   ```
2. Navigate to the project directory:
   ```bash
   cd Colorfull-Ball-with-Particle-Effect-Animation
   ```

## Building

This project uses `make` for building. To build the project, run the following command in your terminal:

3. Compile the code:
   ```bash
    make
   ```
   This command will compile `colorfull_ball.cc` and create an executable named `Colorfull-Ball-with-Particle-Effect-Animation` (or `main.exe` on Windows).

4. Run the executable:

* **Linux/macOS:**
   ```bash
   ./Colorfull-Ball-with-Particle-Effect-Animation
   ```

* **Windows:**
   ```bash
   main.exe
   ```
   (Alternatively, you might directly run `Colorfull-Ball-with-Particle-Effect-Animation.exe` if Makefile is configured to produce that name on Windows)

5. To clean up the build artifacts (object files and executable):
   ```bash
   make clean
   ```

## Features

* **Classic Pong Gameplay:** Faithful recreation of the original Pong game mechanics.
* **Dynamic Ball Color:** The ball's color dynamically shifts through the hue spectrum, adding visual interest.
* **Particle Effects:** Upon collision with paddles, the ball emits colorful particles, enhancing the visual feedback of each hit.
* **Score Keeping:**  Displays the score for both the player and the AI opponent.
* **AI Opponent:**  Includes a simple AI opponent that controls the left paddle.
* **Speed Increment:** Ball speed slightly increases after each paddle hit, making the game progressively more challenging.

## Key Controls

| Control             | Key        | Description                     |
| ------------------- | ---------- | ------------------------------- |
| **Exit Simulation** | `ESC` key  | Closes the game window and exits |
| **Player Paddle Up**   | `Up Arrow` | Moves the player's paddle up     |
| **Player Paddle Down** | `Down Arrow`| Moves the player's paddle down   |

## Code Structure

The project is structured as follows:

* **`colorfull_ball.cc`**: Contains all the source code for the Dynamic Pong game.
    * **SDL Initialization and Setup**: Handles window creation, renderer setup, and initialization of SDL and SDL_ttf.
    * **Game Loop**:  Manages the main game loop, event handling, game state updates, and rendering.
    * **Rendering Functions**: Includes functions for drawing filled circles (`SDL_RenderFillCircle`), and converting HSV color values to RGB (`HSVtoRGB`).
    * **Collision Detection**: Implements circle-rectangle collision detection (`Collision` function).
    * **Particle System**: Defines a `Particle` structure and logic for particle generation, update, and rendering for collision effects.
    * **Game Logic**:  Contains the game's logic for ball movement, paddle control (player and AI), scorekeeping, and ball reset.
    * **Score Rendering**: Uses SDL_ttf to render and display the player and AI scores on the screen.
    * **Dynamic Color**: Implements hue shifting for the ball and particles to create dynamic colors.

* **`Makefile`**:  A build script that simplifies the compilation process. It includes:
    * **Compilation command** for `colorfull_ball.cc` into an executable.
    * **Linking** against necessary SDL2 and SDL2_ttf libraries.
    * **`clean` target** to remove compiled files, keeping the directory clean.

## Demo Video
Check out the project demo video on YouTube: [https://www.youtube.com/watch?v=McOcbGHyAWA](https://www.youtube.com/watch?v=McOcbGHyAWA)

## License

This project is licensed under the MIT License. Feel free to use, modify, and distribute the code.

## Acknowledgements

- [SDL2](https://www.libsdl.org/) for providing a cross-platform API for graphics, input, and audio.
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/) for the TrueType font rendering library used for displaying text in the game.
```
