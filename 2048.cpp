#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
using namespace std;
using namespace sf;

// ---------------- AUDIO GLOBALS ----------------
SoundBuffer moveBuffer;
SoundBuffer mergeBuffer;
SoundBuffer gameOverBuffer;

Sound moveSound;
Sound mergeSound;
Sound GameOverSound;
// -----------------------------------------------

//  BEST SCORE
int LoadBestScore()
{
    int best = 0;
    ifstream fin("HighestScore.txt");
    if (fin.is_open()) { fin >> best; fin.close(); }
    return best;
}

void SaveBestScore(int best)
{
    ofstream fout("HighestScore.txt");
    fout << best;
    fout.close();
}

//  SAVE/LOAD GAME 
void SaveGame(int grid[7][7], int score, int SavedTime)
{
    ofstream fout("SavedGame.txt");
    fout << score << " " << SavedTime << endl;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++)
            fout << grid[i][j] << " ";
        fout << endl;
    }
    fout.close();
}

bool LoadGame(int grid[7][7], int& score, int& SavedTime)
{
    ifstream fin("SavedGame.txt");
    if (!fin.is_open()) return false;
    fin >> score >> SavedTime;
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            fin >> grid[i][j];
    fin.close();
    return true;
}

//  SPAWN TILES 
void SpawnTiles(int grid[7][7])
{
    int emptycount = 0;
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            if (grid[i][j] == 0) emptycount++;
    if (emptycount == 0) return;

    int random = (rand() % emptycount) + 1;
    int count = 0;
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
        {
            if (grid[i][j] == 0) count++;
            if (count == random)
            {
                grid[i][j] = (rand() % 10 == 0) ? 4 : 2;
                return;
            }
        }
}

//  MOVEMENT 
void MoveLeft(int grid[7][7], int& score)
{
    for (int i = 0; i < 7; i++)
    {
        int lastMergePos = -1;
        for (int j = 1; j < 7; j++)
        {
            if (grid[i][j] == 0) continue;
            int k = j;
            while (k > 0 && grid[i][k - 1] == 0)
            {
                grid[i][k - 1] = grid[i][k];
                grid[i][k] = 0;
                k--;
            }
            if (k > 0 && grid[i][k - 1] == grid[i][k] && lastMergePos != k - 1)
            {
                grid[i][k - 1] *= 2;
                score += grid[i][k - 1];
                grid[i][k] = 0;
                lastMergePos = k - 1;

                // play merge sound
                mergeSound.play();
            }
        }
    }
}

void MoveRight(int grid[7][7], int& score)
{
    for (int i = 0; i < 7; i++)
    {
        int lastMergePos = 7;
        for (int j = 5; j >= 0; j--)
        {
            if (grid[i][j] == 0) continue;
            int k = j;
            while (k < 6 && grid[i][k + 1] == 0)
            {
                grid[i][k + 1] = grid[i][k];
                grid[i][k] = 0;
                k++;
            }
            if (k < 6 && grid[i][k + 1] == grid[i][k] && lastMergePos != k + 1)
            {
                grid[i][k + 1] *= 2;
                score += grid[i][k + 1];
                grid[i][k] = 0;
                lastMergePos = k + 1;

                // play merge sound
                mergeSound.play();
            }
        }
    }
}

void MoveUp(int grid[7][7], int& score)
{
    for (int j = 0; j < 7; j++)
    {
        int lastMergePos = -1;
        for (int i = 1; i < 7; i++)
        {
            if (grid[i][j] == 0) continue;
            int k = i;
            while (k > 0 && grid[k - 1][j] == 0)
            {
                grid[k - 1][j] = grid[k][j];
                grid[k][j] = 0;
                k--;
            }
            if (k > 0 && grid[k - 1][j] == grid[k][j] && lastMergePos != k - 1)
            {
                grid[k - 1][j] *= 2;
                score += grid[k - 1][j];
                grid[k][j] = 0;
                lastMergePos = k - 1;

                // play merge sound
                mergeSound.play();
            }
        }
    }
}

void MoveDown(int grid[7][7], int& score)
{
    for (int j = 0; j < 7; j++)
    {
        int lastMergePos = 7;
        for (int i = 5; i >= 0; i--)
        {
            if (grid[i][j] == 0) continue;
            int k = i;
            while (k < 6 && grid[k + 1][j] == 0)
            {
                grid[k + 1][j] = grid[k][j];
                grid[k][j] = 0;
                k++;
            }
            if (k < 6 && grid[k + 1][j] == grid[k][j] && lastMergePos != k + 1)
            {
                grid[k + 1][j] *= 2;
                score += grid[k + 1][j];
                grid[i][j] = 0;
                lastMergePos = k + 1;

                // play merge sound
                mergeSound.play();
            }
        }
    }
}

//  CHECK WIN 
bool CheckWin(int grid[7][7])
{
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            if (grid[i][j] == 2048)
                return true;
    return false;
}

//  CHECK LOSE 
bool CheckLose(int grid[7][7])
{
    // Check if there are any empty tiles
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            if (grid[i][j] == 0)
                return false;

    // Check if any adjacent tiles can merge (horizontal)
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 6; j++)
            if (grid[i][j] == grid[i][j + 1])
                return false;

    // Check if any adjacent tiles can merge (vertical)
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 7; j++)
            if (grid[i][j] == grid[i + 1][j])
                return false;

    return true;
}

//  COLOR 
Color GetColor(int value)
{
    switch (value)
    {
    case 0: return Color(205, 193, 180);
    case 2: return Color(238, 228, 218);
    case 4: return Color(237, 224, 200);
    case 8: return Color(255, 178, 102);
    case 16: return Color(255, 142, 71);
    case 32: return Color(255, 111, 71);
    case 64: return Color(255, 69, 58);
    case 128: return Color(255, 214, 102);
    case 256: return Color(255, 193, 71);
    case 512: return Color(255, 172, 51);
    case 1024: return Color(255, 149, 0);
    case 2048: return Color(255, 204, 0);
    default: return Color::Black;
    }
}

//  RESET GRID 
void ResetGrid(int grid[7][7])
{
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            grid[i][j] = 0;
}

//  STARTUP MENU 
void RunStartupMenu(RenderWindow& window, Sprite& startupBackground, Text startupText[2],
    int& StartupIndex, int& StartupOptionsCount, bool& StartupSelected,
    int grid[7][7], int& score, int& savedTime, Clock& clock, int& BestScore)
{
    while (!StartupSelected && window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed) window.close();
            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Up) StartupIndex = (StartupIndex - 1 + StartupOptionsCount) % StartupOptionsCount;
                if (e.key.code == Keyboard::Down) StartupIndex = (StartupIndex + 1) % StartupOptionsCount;
                if (e.key.code == Keyboard::Enter)
                {
                    StartupSelected = true;
                    if (StartupOptionsCount == 2 && StartupIndex == 1)
                    {
                        LoadGame(grid, score, savedTime);
                    }
                    else
                    {
                        ResetGrid(grid);
                        SpawnTiles(grid);
                        SpawnTiles(grid);
                        score = 0;
                        savedTime = 0;
                        clock.restart();
                        BestScore = 0;
                        SaveBestScore(BestScore);

                        if (remove("SavedGame.txt") == 0)
                            cout << "Saved Game cleared" << endl;
                        else
                            cout << "No Saved data existed" << endl;
                    }
                }
            }
        }

        window.clear(Color(250, 248, 239));
        window.draw(startupBackground);
        for (int i = 0; i < StartupOptionsCount; i++)
        {
            startupText[i].setFillColor(i == StartupIndex ? Color(237, 194, 46) : Color::Black);
            window.draw(startupText[i]);
        }
        window.display();
    }
}

//  HANDLE GAME INPUT 
void HandleGameInput(Event& e, int grid[7][7], int& score, bool& hasWon, bool& hasLost,
    bool& isPaused, bool& showExitConfirm, int& menuIndex, int menuCount,
    int& exitIndex, int& savedTime, Clock& saveClock, bool& showSavedNotification,
    RenderWindow& window, Sprite& startupBackground, Text startupText[2],
    int& StartupIndex, int& StartupOptionsCount, bool& StartupSelected, Clock& clock, int& BestScore)
{
    if (e.key.code == Keyboard::Escape)
    {
        if (hasWon || hasLost)
        {
            // Return to main menu
            hasWon = false;
            hasLost = false;
            score = 0;
            savedTime = 0;
            ResetGrid(grid);

            // Re-enter startup menu
            StartupIndex = 0;
            StartupOptionsCount = 1;
            ifstream checkSave("SavedGame.txt");
            if (checkSave.is_open()) {
                StartupOptionsCount = 2;
                checkSave.close();
            }

            StartupSelected = false;
            RunStartupMenu(window, startupBackground, startupText, StartupIndex, StartupOptionsCount,
                StartupSelected, grid, score, savedTime, clock, BestScore);
        }
        else
        {
            isPaused = !isPaused;
        }
    }

    if (!isPaused && !hasWon && !hasLost)
    {
        bool moved = false;
        if (e.key.code == Keyboard::Left) { MoveLeft(grid, score); moved = true; }
        if (e.key.code == Keyboard::Right) { MoveRight(grid, score); moved = true; }
        if (e.key.code == Keyboard::Up) { MoveUp(grid, score); moved = true; }
        if (e.key.code == Keyboard::Down) { MoveDown(grid, score); moved = true; }

        if (moved)
        {
            // play move sound once per move
            moveSound.play();

            SpawnTiles(grid);
            if (CheckWin(grid))
            {
                GameOverSound.play();
                hasWon = true;
            }
            else if (CheckLose(grid))
            {
                GameOverSound.play();
                hasLost = true;
            }
        }
    }
    else if (isPaused)
    {
        // Pause menu navigation
        if (!showExitConfirm)
        {
            if (e.key.code == Keyboard::Up) menuIndex = (menuIndex - 1 + menuCount) % menuCount;
            if (e.key.code == Keyboard::Down) menuIndex = (menuIndex + 1) % menuCount;
            if (e.key.code == Keyboard::Enter)
            {
                switch (menuIndex)
                {
                case 0: isPaused = false; break; // Resume
                case 1: // Reset
                    score = 0; savedTime = 0; hasWon = false; hasLost = false;
                    ResetGrid(grid);
                    SpawnTiles(grid); SpawnTiles(grid);
                    isPaused = false;
                    break;
                case 2:
                    SaveGame(grid, score, savedTime);
                    showSavedNotification = true;
                    saveClock.restart();
                    break;
                case 3: LoadGame(grid, score, savedTime);
                    hasWon = false;
                    hasLost = false;
                    isPaused = false;
                    break;
                case 4: showExitConfirm = true; exitIndex = 0; break;
                }
            }
        }
        else
        {
            // Exit confirm
            if (e.key.code == Keyboard::Left) exitIndex = (exitIndex - 1 + 2) % 2;
            if (e.key.code == Keyboard::Right) exitIndex = (exitIndex + 1) % 2;
            if (e.key.code == Keyboard::Enter)
            {
                if (exitIndex == 0) window.close();
                else showExitConfirm = false;
            }
        }
    }
}

//  UPDATE GAME TEXTS 
void UpdateGameTexts(Text& scoreText, Text& bestText, Text& timerText, int score,
    int& BestScore, int savedTime, Clock& clock, bool isPaused)
{
    scoreText.setString("Score: " + to_string(score));
    if (score > BestScore)
    {
        BestScore = score;
        SaveBestScore(BestScore);
    }
    bestText.setString("Best: " + to_string(BestScore));

    int total = 0;
    if (!isPaused)
    {
        total = savedTime + clock.getElapsedTime().asSeconds();
    }
    else
    {
        total = savedTime;
    }
    int minutes = total / 60;
    int seconds = total % 60;
    timerText.setString("Timer: " + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds));
}

//  DRAW GAME GRID 
void DrawGameGrid(RenderWindow& window, int grid[7][7], Font& fontUI, float scaleX, float scaleY,
    float boxX, float boxY, float TILE_SIZE, float padding, float tiledraw)
{
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            RectangleShape tile(Vector2f(tiledraw, tiledraw));
            tile.setPosition(boxX + j * TILE_SIZE + padding / 2, boxY + i * TILE_SIZE + padding / 2);
            tile.setFillColor(GetColor(grid[i][j]));
            window.draw(tile);

            if (grid[i][j] != 0)
            {
                Text numberText;
                numberText.setFont(fontUI);
                numberText.setFillColor(Color::Black);
                numberText.setString(to_string(grid[i][j]));
                numberText.setCharacterSize(40 * min(scaleX, scaleY));

                string s = to_string(grid[i][j]);
                numberText.setString(s);

                int len = s.length();

                float posX = tile.getPosition().x;
                float posY = tile.getPosition().y;

                float centerX = posX + tiledraw / 2;
                float centerY = posY + tiledraw / 2;

                if (len == 1)
                    numberText.setPosition(centerX - 12 * min(scaleX, scaleY), centerY - 20 * min(scaleX, scaleY));
                else if (len == 2)
                    numberText.setPosition(centerX - 22 * min(scaleX, scaleY), centerY - 20 * min(scaleX, scaleY));
                else if (len == 3)
                    numberText.setPosition(centerX - 32 * min(scaleX, scaleY), centerY - 20 * min(scaleX, scaleY));
                else if (len == 4)
                    numberText.setPosition(centerX - 42 * min(scaleX, scaleY), centerY - 20 * min(scaleX, scaleY));

                window.draw(numberText);
            }
        }
    }
}

//  DRAW PAUSE MENU 
void DrawPauseMenu(RenderWindow& window, RectangleShape& menuBackground, Text menuText[5],
    Text exitText[2], Font& fontUI, int menuIndex, int menuCount,
    int exitIndex, bool showExitConfirm)
{
    window.draw(menuBackground);
    if (!showExitConfirm)
    {
        for (int i = 0; i < menuCount; i++)
            menuText[i].setFillColor(i == menuIndex ? Color(237, 194, 46) : Color::Black), window.draw(menuText[i]);
    }
    else
    {
        Text confirm;
        confirm.setFont(fontUI);
        confirm.setCharacterSize(35);
        confirm.setFillColor(Color::Black);
        confirm.setString("Are you sure?");

        float confirmWidth = confirm.getLocalBounds().width;
        float confirmX = menuBackground.getPosition().x + (menuBackground.getSize().x - confirmWidth) / 2.0f;
        float confirmY = menuBackground.getPosition().y + 120;
        confirm.setPosition(confirmX, confirmY);
        window.draw(confirm);

        for (int i = 0; i < 2; i++)
            exitText[i].setFillColor(i == exitIndex ? Color(237, 194, 46) : Color::Black), window.draw(exitText[i]);
    }
}

//  DRAW SAVED NOTIFICATION 
void DrawSavedNotification(RenderWindow& window, Font& fontUI, float scaleX, float scaleY,
    float boxX, float boxY, float BOX_SIZE, Clock& saveClock, bool& showSavedNotification)
{
    Text savedText;
    savedText.setFont(fontUI);
    savedText.setCharacterSize(50 * min(scaleX, scaleY));
    savedText.setFillColor(Color::Green);
    savedText.setString("Saved Game!");

    float posX = boxX + (BOX_SIZE - savedText.getLocalBounds().width) / 2;
    float posY = boxY - savedText.getLocalBounds().height - 10 * min(scaleX, scaleY);

    savedText.setPosition(posX, posY);

    window.draw(savedText);

    // Hide after 2 seconds
    if (saveClock.getElapsedTime().asSeconds() > 2.0f)
        showSavedNotification = false;
}

//  DRAW WIN SCREEN 
void DrawWinScreen(RenderWindow& window, Font& fontUI, int score)
{
    RectangleShape winBackground(Vector2f(600, 300));
    winBackground.setFillColor(Color(237, 194, 46, 240));

    // Center the yellow box
    float winBgX = (window.getSize().x - 600) / 2.0f;
    float winBgY = (window.getSize().y - 300) / 2.0f;
    winBackground.setPosition(winBgX, winBgY);
    window.draw(winBackground);

    Text winText;
    winText.setFont(fontUI);
    winText.setCharacterSize(80);
    winText.setFillColor(Color::Black);
    winText.setString("YOU WIN!");

    float winTextWidth = winText.getLocalBounds().width;
    winText.setPosition((window.getSize().x - winTextWidth) / 2.0f, winBgY + 50);
    window.draw(winText);

    Text winScore;
    winScore.setFont(fontUI);
    winScore.setCharacterSize(40);
    winScore.setFillColor(Color::Black);
    winScore.setString("Score: " + to_string(score));

    float winScoreWidth = winScore.getLocalBounds().width;
    winScore.setPosition((window.getSize().x - winScoreWidth) / 2.0f, winBgY + 150);
    window.draw(winScore);

    Text pressEsc;
    pressEsc.setFont(fontUI);
    pressEsc.setCharacterSize(25);
    pressEsc.setFillColor(Color::Black);
    pressEsc.setString("Press ESC for main menu");

    float pressEscWidth = pressEsc.getLocalBounds().width;
    pressEsc.setPosition((window.getSize().x - pressEscWidth) / 2.0f, winBgY + 230);
    window.draw(pressEsc);
}

//  DRAW LOSE SCREEN 
void DrawLoseScreen(RenderWindow& window, Font& fontUI, int score)
{
    RectangleShape loseBackground(Vector2f(600, 300));
    loseBackground.setFillColor(Color(205, 193, 180, 240));

    // Center the gray box
    float loseBgX = (window.getSize().x - 600) / 2.0f;
    float loseBgY = (window.getSize().y - 300) / 2.0f;
    loseBackground.setPosition(loseBgX, loseBgY);
    window.draw(loseBackground);

    Text loseText;
    loseText.setFont(fontUI);
    loseText.setCharacterSize(80);
    loseText.setFillColor(Color(119, 110, 101));
    loseText.setString("YOU LOSE!");

    float loseTextWidth = loseText.getLocalBounds().width;
    loseText.setPosition((window.getSize().x - loseTextWidth) / 2.0f, loseBgY + 50);
    window.draw(loseText);

    Text loseScore;
    loseScore.setFont(fontUI);
    loseScore.setCharacterSize(40);
    loseScore.setFillColor(Color(119, 110, 101));
    loseScore.setString("Score: " + to_string(score));

    float loseScoreWidth = loseScore.getLocalBounds().width;
    loseScore.setPosition((window.getSize().x - loseScoreWidth) / 2.0f, loseBgY + 150);
    window.draw(loseScore);

    Text pressEscLose;
    pressEscLose.setFont(fontUI);
    pressEscLose.setCharacterSize(25);
    pressEscLose.setFillColor(Color(119, 110, 101));
    pressEscLose.setString("Press ESC for main menu");

    float pressEscLoseWidth = pressEscLose.getLocalBounds().width;
    pressEscLose.setPosition((window.getSize().x - pressEscLoseWidth) / 2.0f, loseBgY + 230);
    window.draw(pressEscLose);
}

//  UPDATE PAUSE TIMER 
void UpdatePauseTimer(int& savedTime, Clock& clock)
{
    savedTime += clock.getElapsedTime().asSeconds();
    clock.restart();
}

int main()
{
    srand(time(0));
    RenderWindow window(VideoMode(1920, 1080), "2048", Style::Default);
    window.setVerticalSyncEnabled(true);

    float scaleX = window.getSize().x / 1500.0f;
    float scaleY = window.getSize().y / 1050.0f;

    //  FONTS 
    Font fontUI;
    if (!fontUI.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        cout << "Failed to load font!" << endl;

    // ---------------- AUDIO LOAD ----------------
    SoundBuffer moveBuffer, mergeBuffer, gameOverBuffer;

    if (!moveBuffer.loadFromFile("C:/Users/mueez/Downloads/merge.wav"))
        cout << "Failed to load merge.wav!" << endl;

    if (!mergeBuffer.loadFromFile("C:/Users/mueez/Downloads/merge.wav"))
        cout << "Failed to load merge.wav!" << endl;

    if (!gameOverBuffer.loadFromFile("C:/Users/mueez/Downloads/finish.wav"))
        cout << "Failed to load finish.wav!" << endl;

    moveSound.setBuffer(moveBuffer);
    mergeSound.setBuffer(mergeBuffer);
    GameOverSound.setBuffer(gameOverBuffer);
    GameOverSound.setVolume(70.f);

    // optional volumes
    moveSound.setVolume(40.f);
    mergeSound.setVolume(60.f);
    // --------------------------------------------

    //  VARIABLES 
    const int GRID_SIZE = 7;
    int grid[7][7] = { 0 };

    float baseBoxSize = 700.0f;
    float BOX_SIZE = baseBoxSize * min(scaleX, scaleY);
    float TILE_SIZE = BOX_SIZE / GRID_SIZE;
    float padding = 5.0f * min(scaleX, scaleY);
    float tiledraw = TILE_SIZE - padding;
    float boxX = (window.getSize().x - BOX_SIZE) / 2.0f;
    float boxY = (window.getSize().y - BOX_SIZE) / 2.0f;

    int score = 0;
    int savedTime = 0;
    int BestScore = LoadBestScore();
    bool hasSavedGame = false;
    bool showSavedNotification = false;
    bool hasWon = false;
    bool hasLost = false;
    Clock saveClock;

    ifstream fin("SavedGame.txt");
    if (fin.is_open()) { hasSavedGame = true; fin.close(); }

    // TEXTS 
    Text name, scoreText, bestText, timerText;
    name.setFont(fontUI);
    name.setCharacterSize(20 * min(scaleX, scaleY));
    name.setFillColor(Color::Black);
    name.setPosition(10, 10);

    scoreText.setFont(fontUI);
    scoreText.setCharacterSize(20 * min(scaleX, scaleY));
    scoreText.setFillColor(Color::Black);
    scoreText.setPosition(700, 10);

    bestText.setFont(fontUI);
    bestText.setCharacterSize(20 * min(scaleX, scaleY));
    bestText.setFillColor(Color::Black);
    bestText.setPosition(900, 10);

    timerText.setFont(fontUI);
    timerText.setCharacterSize(20 * min(scaleX, scaleY));
    timerText.setFillColor(Color::Black);
    timerText.setPosition(1200, 10);

    string username;
    cout << "Enter your name: "; getline(cin, username);
    name.setString("Player: " + username);

    Clock clock;

    // STARTUP BACKGROUND 
    Texture startupBackgroundTexture;
    if (!startupBackgroundTexture.loadFromFile("C:/Users/mueez/Downloads/2048.jpeg"))
        cout << "Failed to load image!" << endl;

    Sprite startupBackground(startupBackgroundTexture);
    float bgScaleX = window.getSize().x / (float)startupBackgroundTexture.getSize().x;
    float bgScaleY = window.getSize().y / (float)startupBackgroundTexture.getSize().y;
    startupBackground.setScale(bgScaleX, bgScaleY);

    // STARTUP MENU 
    int StartupIndex = 0;
    int StartupOptionsCount = (hasSavedGame ? 2 : 1);
    string StartupOptions[2] = { "New Game", "Load Game" };
    Text startupText[2];
    for (int i = 0; i < StartupOptionsCount; i++)
    {
        startupText[i].setFont(fontUI);
        startupText[i].setCharacterSize(60);
        startupText[i].setFillColor(Color::Black);
        startupText[i].setString(StartupOptions[i]);

        float textWidth = startupText[i].getLocalBounds().width;
        float centerX = (window.getSize().x - textWidth) / 2.0f;
        startupText[i].setPosition(centerX, 400 + i * 100);
    }

    bool StartupSelected = false;
    RunStartupMenu(window, startupBackground, startupText, StartupIndex, StartupOptionsCount,
        StartupSelected, grid, score, savedTime, clock, BestScore);

    //Pause Menu Variables
    bool isPaused = false, showExitConfirm = false;
    int menuIndex = 0, menuCount = 5, exitIndex = 0;
    string menuStrings[5] = { "Resume", "Reset Game", "Save Game", "Load Game", "Exit" };
    Text menuText[5], exitText[2];
    RectangleShape menuBackground(Vector2f(400, 400));
    menuBackground.setFillColor(Color(200, 200, 200, 220));

    float menuBgX = (window.getSize().x - 400) / 2.0f;
    float menuBgY = (window.getSize().y - 400) / 2.0f;
    menuBackground.setPosition(menuBgX, menuBgY);

    for (int i = 0; i < 5; i++)
    {
        menuText[i].setFont(fontUI);
        menuText[i].setCharacterSize(40 * min(scaleX, scaleY));
        menuText[i].setFillColor(Color::Black);
        menuText[i].setString(menuStrings[i]);

        float textWidth = menuText[i].getLocalBounds().width;
        float centerX = menuBackground.getPosition().x + (menuBackground.getSize().x - textWidth) / 2.0f;
        float startY = menuBackground.getPosition().y + 50;
        menuText[i].setPosition(centerX, startY + i * 50);
    }

    string exitOptions[2] = { "Yes", "No" };
    for (int i = 0; i < 2; i++)
    {
        exitText[i].setFont(fontUI);
        exitText[i].setCharacterSize(40 * min(scaleX, scaleY));
        exitText[i].setFillColor(Color::Black);
        exitText[i].setString(exitOptions[i]);
        float text0Width = exitText[0].getLocalBounds().width;
        float text1Width = exitText[1].getLocalBounds().width;
        float totalWidth = text0Width + 100 + text1Width;
        float startX = menuBackground.getPosition().x + (menuBackground.getSize().x - totalWidth) / 2.0f;
        float startY = menuBackground.getPosition().y + 220;

        if (i == 0)
            exitText[i].setPosition(startX, startY);
        else
            exitText[i].setPosition(startX + text0Width + 100, startY);
    }

    //Game Loop
    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed) window.close();
            if (e.type == Event::KeyPressed)
            {
                HandleGameInput(e, grid, score, hasWon, hasLost, isPaused, showExitConfirm,
                    menuIndex, menuCount, exitIndex, savedTime, saveClock,
                    showSavedNotification, window, startupBackground, startupText,
                    StartupIndex, StartupOptionsCount, StartupSelected, clock, BestScore);
            }
        }

        // Update pause timer
        if (isPaused)
        {
            UpdatePauseTimer(savedTime, clock);
        }

        // UPDATE TEXTS
        UpdateGameTexts(scoreText, bestText, timerText, score, BestScore, savedTime, clock, isPaused);

        // DRAW
        window.clear(Color(250, 248, 239));
        window.draw(name);
        window.draw(scoreText);
        window.draw(bestText);
        window.draw(timerText);

        DrawGameGrid(window, grid, fontUI, scaleX, scaleY, boxX, boxY, TILE_SIZE, padding, tiledraw);

        // Draw pause menu
        if (isPaused)
        {
            DrawPauseMenu(window, menuBackground, menuText, exitText, fontUI, menuIndex,
                menuCount, exitIndex, showExitConfirm);
        }

        // Draw saved notification
        if (showSavedNotification)
        {
            DrawSavedNotification(window, fontUI, scaleX, scaleY, boxX, boxY, BOX_SIZE,
                saveClock, showSavedNotification);
        }

        // Draw win screen
        if (hasWon)
        {
            DrawWinScreen(window, fontUI, score);
        }

        // Draw lose screen
        if (hasLost)
        {
            DrawLoseScreen(window, fontUI, score);
        }

        window.display();
    }

    return 0;
}