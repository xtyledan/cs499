# Brick Breaker Game

## Overview
This project is a modernized implementation of a classic brick breaker game written in C++ with OpenGL for rendering and GLFW for window and input handling.  
Originally developed as a single monolithic file, the codebase has been refactored into modular components that separate game logic, rendering, input, and entity management.  
The game demonstrates skills in software design, algorithms, data structures, and database integration, and is included as an artifact for my ePortfolio.

## Features
- **Modular Design**: Clear separation of classes such as `Brick`, `Circle` (ball), `Paddle`, and supporting systems like `Collision`, `Grid`, and `Level`.
- **Modern C++ Practices**: Use of `enum class`, smart pointers, `const` correctness, `<chrono>` for delta time, and `<random>` for reproducible randomness.
- **Algorithms & Data Structures**:  
  - Spatial partitioning grid to reduce collision checks from O(n*m) to localized queries.  
  - Continuous collision detection to prevent tunneling issues.  
- **Graphics**:  
  - OpenGL rendering via GLFW context.  
  - Future-proofed renderer class to allow migration away from immediate mode to modern OpenGL.  
- **Persistence with SQLite**:  
  - Scores table storing player name, points, date, and duration.  
  - Settings table for adjustable parameters like window size and difficulty.  
  - Data access layer using prepared statements for security.  
  - In-game leaderboard that displays top scores.  
- **Testing & Tooling**:  
  - Unit tests for collision logic with GoogleTest.  
  - Integration tests for database operations.  
  - Configurable build with CMake for cross-platform support.  
  - Static analysis and formatting configuration applied.

## Controls
- **Arrow Keys**: Move paddle left and right.  
- **Spacebar**: Launch the ball (or spawn a new one if attached).  
- **Escape**: Exit the game.  
- **P**: Pause/unpause gameplay.

## Installation & Setup

### Prerequisites
- Visual Studio 2022 (or a modern C++17 compiler)
- CMake (if building outside of Visual Studio)
- OpenGL development libraries
- [GLFW 3.4](https://www.glfw.org/) and [GLEW 2.2](http://glew.sourceforge.net/) binaries
- SQLite3 (via vcpkg, amalgamation source, or prebuilt DLL/lib)

### Building
1. Clone the repository.
2. Open the solution in Visual Studio or configure with CMake.
3. Make sure the following include paths are set in project properties:
   - `glfw-3.4.bin.WIN64/include`
   - `glew-2.2.0/include`
   - SQLite headers (from amalgamation or vcpkg)
4. Link against:
   - `glfw3dll.lib` or `glfw3_mt.lib`
   - `glew32.lib` or `glew32s.lib`
   - `opengl32.lib`
   - `sqlite3.lib` (if not building from `sqlite3.c` directly)
5. Copy the required DLLs (`glfw3.dll`, `glew32.dll`, `sqlite3.dll`) next to your executable if using dynamic builds.
6. Build and run the project.

### Database Setup
On first run, the game creates `game.db` in the executable folder with the required schema:
- `scores` table for leaderboard entries
- `settings` table for key-value configuration

You can inspect the database with the SQLite CLI:
```sh
sqlite3 game.db
.tables
SELECT * FROM scores ORDER BY points DESC LIMIT 10;
