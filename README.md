# RIVER-INF - Game Development Project

## Description
RIVER-INF is an implementation of the classic Atari 2600 game "River Raid" (1982) created by Carol Shaw. This is a course project for INF01202 - Algoritmos e Programação.

## Game Overview
- **Objective**: Navigate an aircraft vertically through a river, shooting enemies and avoiding obstacles
- **Enemies**: Ships (30 pts), Helicopters (60 pts), Bridges (200 pts)
- **Obstacles**: Terrain/Forest (T), Islands
- **Fuel**: Collect fuel stations to recharge (G)
- **Lives**: Start with 3, gain extra every 1000 points

## Controls
- **A / LEFT ARROW**: Move aircraft left
- **D / RIGHT ARROW**: Move aircraft right
- **K / SPACE**: Shoot bullets
- **ENTER**: Pause/Resume game
- **R**: Restart level
- **ESC**: Quit game

## Game Features
- Multiple phases loaded from text files (fase1.txt, fase2.txt, etc.)
- High score system saved to binary file (highscore.bin)
- Score tracking and fuel management
- Collision detection for player, enemies, and terrain
- Main menu with high score display

## Map Format
- **Size**: 24x20 characters
- **Pixel Size**: 960x800 (each character = 40x40 pixels)
- **Characters**:
  - `A`: Player aircraft
  - `T`: Terrain/Forest
  - `N`: Enemy ship
  - `X`: Enemy helicopter
  - `P`: Bridge (destroyable obstacle)
  - `G`: Fuel station
  - ` ` (space): River

## Installation Requirements
- Raylib library (https://www.raylib.com/)
- C compiler (GCC, MSVC, Clang)
- CMake (optional, for building)

## Compilation
\`\`\`bash
# With GCC
gcc -o river_inf main.c map_loader.c game_logic.c input.c render.c -lraylib -lm

# On Windows with MinGW
gcc -o river_inf.exe main.c map_loader.c game_logic.c input.c render.c -lraylib -lm -lopengl32 -lgdi32 -lwinmm
\`\`\`

## File Structure
- `game.h`: Main header with game structures and function declarations
- `map_loader.c`: Map loading and game state management
- `game_logic.c`: Core game mechanics (collisions, scoring, etc.)
- `input.c`: Input handling and control system
- `render.c`: Rendering and display functions
- `main.c`: Main game loop
- `highscore.bin`: High score data (binary file)
- `fase*.txt`: Level maps (text files)

## Game Mechanics
1. **Movement**: Aircraft moves up automatically, controlled horizontally by player
2. **Fuel**: Depletes over time, collected at fuel stations
3. **Scoring**: Destroy enemies for points, bonus life every 1000 points
4. **Collisions**: 
   - Hitting terrain/islands = lose 1 life
   - Hitting enemies = lose 1 life
   - Hitting fuel station = refuel (30 units, max 200)
5. **Level Progression**: Destroy bridges (P) or reach top of map to advance

## Extra Features (Bonus Points)
- Pause functionality (ENTER key)
- Level restart (R key)
- High score system with persistent storage
- Quit confirmation dialog

## Notes
- This project was created for educational purposes as part of INF01202
- Inspired by the classic River Raid arcade game
- No AI tools were used in development
- Team members should be prepared to explain any part of the code
