# 2048 Game - C++ Implementation with SFML

## Overview
This is a custom implementation of the popular **2048 puzzle game** built in C++ using the **SFML (Simple and Fast Multimedia Library)**. The game runs on a 7x7 grid (extended from the original 4x4) with full graphics, audio, and save/load functionality.

---

## Key Features

### Game Mechanics
- **7x7 Grid Gameplay**: Extended grid size for more challenging gameplay
- **Tile Movement**: Move tiles in four directions (Left, Right, Up, Down)
- **Tile Merging**: Identical adjacent tiles merge and double in value
- **Scoring System**: Points awarded when tiles merge
- **Win Condition**: Reach the 2048 tile to win
- **Lose Detection**: Game ends when no moves are possible

### User Interface
- **Startup Menu**: Choose between "New Game" or "Load Game"
- **Pause Menu**: Resume, Reset, Save, Load, or Exit during gameplay
- **HUD Display**: Shows player name, current score, best score, and elapsed timer
- **Win/Lose Screens**: Displays game outcome with final score
- **Notifications**: Visual feedback when game is saved

### Audio System
- **Move Sound**: Plays when a tile is moved
- **Merge Sound**: Plays when tiles merge
- **Game Over Sound**: Plays when win or lose condition is reached
- **Volume Control**: Different volume level for each sound type

### Save/Load System
- **Game Save**: Save current grid state, score, and timer
- **Game Load**: Resume previously saved games
- **Best Score Tracking**: Persistent storage of highest score achieved

---

## Core Concepts Explained

### 1. **Audio System (Global Variables)**
```cpp
SoundBuffer moveBuffer, mergeBuffer, gameOverBuffer;
Sound moveSound, mergeSound, GameOverSound;
```
- **SoundBuffer**: Loads audio files from disk into memory
- **Sound**: Plays audio using a buffer
- Buffers are global for easy access throughout the game

### 2. **Game Grid & State Management**
```cpp
int grid[7][7] = {0};  // 7x7 grid initialized to 0 (empty)
int score = 0;         // Current game score
int BestScore = 0;     // Persistent best score
```
- Grid stores tile values (0 = empty, 2/4/8/16... = tile values)
- State variables track game progress and achievements

### 3. **File I/O Operations**

#### Best Score Persistence
```cpp
int LoadBestScore()    // Reads from HighestScore.txt
void SaveBestScore()   // Writes to HighestScore.txt
```

#### Game Save/Load
```cpp
void SaveGame()        // Saves grid, score, and timer to SavedGame.txt
bool LoadGame()        // Restores game state from file
```

### 4. **Core Game Logic**

#### Tile Spawning
```cpp
void SpawnTiles(int grid[7][7])
```
- Counts empty tiles (value = 0)
- Randomly selects an empty position
- Spawns either 2 (90% chance) or 4 (10% chance)
- Called after each valid move

#### Movement Functions
```cpp
void MoveLeft(int grid[7][7], int& score)
void MoveRight(int grid[7][7], int& score)
void MoveUp(int grid[7][7], int& score)
void MoveDown(int grid[7][7], int& score)
```

**Algorithm**:
1. **Compress**: Move all non-zero tiles toward target direction
2. **Merge**: Combine identical adjacent tiles (only once per row/column per move)
3. **Update Score**: Add merged tile value to score
4. Track `lastMergePos` to prevent merging the same tile twice

**Compression Example (MoveLeft)**:
- Iterate through each row
- For each non-zero tile, move it left while left neighbor is empty
- If left neighbor equals current tile and hasn't been merged, combine them

### 5. **Win/Lose Conditions**

#### Win Detection
```cpp
bool CheckWin(int grid[7][7])
```
- Scans grid for any tile with value 2048
- Returns true when found

#### Lose Detection
```cpp
bool CheckLose(int grid[7][7])
```
- Checks for empty tiles (if any exist, game continues)
- Scans horizontal adjacencies for matching values
- Scans vertical adjacencies for matching values
- Returns true only if grid is full AND no merges possible

### 6. **Color Mapping**
```cpp
Color GetColor(int value)
```
Maps tile values to RGB colors for visual representation:
- 0 (empty): Light gray `(205, 193, 180)`
- 2: Pale yellow `(238, 228, 218)`
- 4: Light beige `(237, 224, 200)`
- ...up to 2048: Gold `(255, 204, 0)`

### 7. **User Input Handling**

#### Startup Menu Navigation
```cpp
void RunStartupMenu()
```
- Arrow keys select options
- Enter key confirms selection
- Loads game or starts fresh based on selection

#### In-Game Input
```cpp
void HandleGameInput(Event& e, ...)
```
- **Arrow Keys**: Move tiles in corresponding direction
- **ESC**: Pause/Resume or return to main menu
- **Pause Menu Navigation**: Up/Down arrows select, Enter confirms

### 8. **Graphics & Rendering**

#### UI Components
- **Font Rendering**: Uses Arial.ttf for text display
- **Tile Rendering**: Dynamic size based on window resolution
- **Text Centering**: Calculated based on text bounds and available space

#### Drawing Functions
```cpp
void DrawGameGrid()           // Renders all tiles and numbers
void DrawPauseMenu()          // Renders pause menu options
void DrawSavedNotification()  // Shows temporary "Saved Game!" message
void DrawWinScreen()          // Shows victory screen
void DrawLoseScreen()         // Shows defeat screen
```

### 9. **Responsive Scaling**
```cpp
float scaleX = window.getSize().x / 1500.0f;
float scaleY = window.getSize().y / 1050.0f;
```
- Game adapts to different screen resolutions
- All UI elements scale proportionally
- Maintains playability on various monitor sizes

### 10. **Game Loop Architecture**
```cpp
while (window.isOpen())
{
    // Poll events (input handling)
    // Update game state
    // Render graphics
    // Display frame
}
```

**Frame Rate**: Vertical sync enabled for smooth 60 FPS gameplay

---

## File Structure

```
MY Projects/
├── 2048.cpp           (Main game implementation - ~1000 lines)
├── Project.cpp        (Other project files)
└── MY Projects.vcxproj
```

### Key Assets Required
- **Font**: `C:/Windows/Fonts/arial.ttf` (Windows system font)
- **Audio Files**:
  - `C:/Users/mueez/Downloads/merge.wav` (Merge sound effect)
  - `C:/Users/mueez/Downloads/finish.wav` (Game over sound)
- **Image**: `C:/Users/mueez/Downloads/2048.jpeg` (Startup menu background)

### Data Files (Auto-generated)
- `HighestScore.txt`: Stores best score achieved
- `SavedGame.txt`: Stores saved game state

---

## Technical Stack

| Component | Technology |
|-----------|-----------|
| Language | C++11 |
| Graphics | SFML Graphics Module |
| Audio | SFML Audio Module |
| File I/O | Standard C++ `<fstream>` |
| Input Handling | SFML Events |
| RNG | `<cstdlib>` `rand()` |

---

## Game States

```
Startup Menu → New Game / Load Game
                    ↓
            Game Loop
            ├── Running (input processed, tiles move)
            ├── Paused (pause menu active)
            ├── Won (2048 tile reached)
            └── Lost (no valid moves)
                    ↓
            Return to Startup Menu
```

---

## Timer System

- **Active Gameplay**: Timer increments in real-time using SFML Clock
- **Paused**: Timer frozen, elapsed time preserved in `savedTime`
- **Display Format**: MM:SS (e.g., "05:32" for 5 minutes 32 seconds)
- **Persistence**: Timer state preserved in save files

---

## Pause Menu Options

| Option | Action |
|--------|--------|
| Resume | Unpause and continue playing |
| Reset Game | Clear grid, reset score to 0, start fresh |
| Save Game | Write current state to SavedGame.txt, show notification |
| Load Game | Restore state from SavedGame.txt |
| Exit | Show confirmation dialog, close game if confirmed |

---

## Key Algorithms

### Tile Movement Algorithm
1. **Direction Selection**: Determine target direction from input
2. **Row/Column Iteration**: Process relevant rows or columns
3. **Sliding Phase**: Move tiles toward boundary until collision
4. **Merge Phase**: Combine tiles of equal value with score update
5. **Spawn Phase**: Add new tile if any movement occurred
6. **Check End State**: Test for win/lose conditions

### Space Complexity
- **Grid Storage**: O(49) = O(1) - Fixed 7x7 array
- **Other Variables**: O(1) - Fixed number of game state variables

### Time Complexity Per Move
- **Movement**: O(49) = O(1) - Fixed grid size
- **Spawn**: O(49) = O(1) - Worst case iterates full grid
- **Win/Lose Check**: O(49) = O(1) - Scans full grid
- **Overall**: O(1) - Constant time per frame

---

## Usage

### Compilation
Use Visual Studio 2019+ with SFML library linked:
1. Ensure SFML dependencies are installed
2. Link against SFML Graphics and Audio libraries
3. Build using Visual Studio project configuration

### Running
```
1. Execute MY Projects.exe
2. Enter player name when prompted
3. Choose "New Game" or "Load Game"
4. Use arrow keys to move tiles
5. Press ESC to pause
6. Reach 2048 to win!
```

### Controls
- **Arrow Keys**: Move tiles
- **ESC**: Pause/Unpause or Return to Menu
- **Enter**: Confirm menu selections
- **Up/Down**: Navigate menus

---

## Design Patterns Used

1. **Game Loop Pattern**: Standard update-render cycle
2. **State Management**: Using boolean flags (isPaused, hasWon, hasLost)
3. **Configuration Values**: Constants for grid size, window dimensions
4. **Separation of Concerns**: Logic functions separate from rendering
5. **Global State**: Audio buffers stored globally for easy access

---

## Potential Improvements

- Add difficulty levels (4x4, 5x5, 8x8 grids)
- Implement undo/redo functionality
- Add leaderboard system
- Touch/mobile controls support
- Animated tile movements
- Particle effects for merges
- Multiple themes/skins
- Sound settings menu

---

## Notes

- **Hard-coded Paths**: Audio files, fonts, and save files use absolute paths specific to developer's machine
- **Grid Size**: Currently fixed at 7x7; could be parameterized for flexibility
- **Error Handling**: Basic file I/O error checking; could be more robust
- **Memory**: No dynamic allocation; all structures are stack-allocated for simplicity

---

## License & Credits

- Game Concept: Original 2048 game by Gabriele Cirulli
- Implementation: Custom C++ with SFML
- SFML Library: © Laurent Gomila
- Built: February 2026

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Font not loading | Verify Arial.ttf exists in Windows Fonts directory |
| Audio not playing | Check audio file paths match actual file locations |
| Save file not found | First playthrough will not have SavedGame.txt; create new game first |
| Tiles not rendering | Ensure SFML Graphics library is properly linked |
| Window not opening | Check VideoMode(1920, 1080) resolution is supported |

---

This README documents a fully-featured 2048 implementation with professional game development practices including save systems, audio integration, and responsive UI design.
