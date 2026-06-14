#include <iostream> 
// input/output

#include <thread> 
/*complex delay command - std::this_thread::sleep_for(std::chrono::milliseconds(ms))
but shortened to this_thread::sleep_for(100ms);*/

#include <cstdlib> 
// for random letters and clearing terminal on loss

#include <vector>
// for lists (map)

#include <conio.h>
// for non-blocking input

#include <chrono>

#include <algorithm>

#ifdef _WIN32
#include <windows.h>  // needed for gotoxy
#endif

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

// Move cursor to position (0,0) - top-left corner
// Works differently on Windows and Linux, so conditional compilation is used
void gotoxy(int x, int y) 
{
#ifdef _WIN32
    // Windows: use SetConsoleCursorPosition
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    // Linux/Mac: use ANSI escape sequences
    // \033[ - start of escape sequence
    // y;xH - moves cursor to row y, column x
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

// Hide cursor (so it doesn't blink)
void hideCursor() 
{
#ifdef _WIN32
    // Windows: hide console cursor
    CONSOLE_CURSOR_INFO cursorInfo;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo); 
#else
    // Linux: ANSI code to hide cursor
    printf("\033[?25l");
#endif
}

using namespace std;
using namespace std::chrono_literals;

// star class
struct Star 
{
    int x, y = 0;
    char symbol = "*+Xx"[rand() % 4];;
    bool active = true;   // hasn't fallen yet
    int type = 0; // 0=normal star, 1=extra life
};

// bonus life class
struct Bonus 
{
    int x, y = 0;
    char symbol = 'H';
    bool active = true;
};

// variables for timing and game itself
vector<Star> stars;
vector<Bonus> bonuses;

int score = 0;
int highScore = 0;
int lives = 3;

int groundY = 10;

int playerX = 5;
int playerY = groundY - 1;

bool gameRunning = true;

using Clock = std::chrono::steady_clock;
int move_delay_ms = 200;   // time between frames
int spawn_star_ms = 1000;  // time between spawning a new star
int spawn_bonus_ms = 3000; // time between spawning a bonus
auto lastFrameTime = Clock::now();

vector<vector<char>> map(10, vector<char>(10, ' '));

// Function draws the game field on the screen
void draw(vector<vector<char>>& map, vector<Star>& stars) 
{
    // draw the game field
    for (int i = 0; i < 10; i++) 
    {
        for (int j = 0; j < 10; j++)
        {
            map[i][j] = ' ';
        }
    }
    
    // draw falling stars
    for (const auto& s : stars) 
    {
        if (s.active && s.y >= 0 && s.y < 10 && s.x >= 0 && s.x < 10) map[s.y][s.x] = s.symbol;
    }

    for (const auto& b : bonuses) 
    {
        if (b.active && b.y >= 0 && b.y < 10 && b.x >= 0 && b.x < 10) map[b.y][b.x] = b.symbol;
    }

    // draw player
    map[playerY][playerX] = 'V';

    // print everything on the map
    for (int i = 0; i < 10; i++) 
    {
        
        for (int j = 0; j < 10; j++) 
        {
            cout << map[i][j];
        }
        cout << endl;
    }

    cout << "Your score: " << score << endl;
    cout << "Your lives: " << lives << endl;
    cout << "Game speed: " << 1000 / move_delay_ms + 95 << "%" << endl;
}


// function to spawn a new star
void spawnStar(vector<Star>& stars) {
    Star newStar;
    newStar.x = rand() % 10;
    newStar.y = 0;
    newStar.symbol = "*+Xx"[rand() % 4]; // different symbols
    newStar.active = true;
    stars.push_back(newStar);
}

void spawnBonus(vector<Bonus>& bonuses) {
    Bonus newBonus;
    newBonus.x = rand() % 10;
    newBonus.y = 0;
    newBonus.symbol = 'h';
    newBonus.active = true;
    bonuses.push_back(newBonus);
}

void GameOver()
{
    system(CLEAR);
    cout << "You lost!" << endl << "Your best score: " << highScore << endl;
    cout << "Press any key to continue..." << endl;
    this_thread::sleep_for(500ms);
    _getch();
}


// main game function
int main()
{   
    #ifdef _WIN32
        system("chcp 65001 > nul");    // Windows: switch console to UTF-8
    #endif
        system(CLEAR);                 // clear screen once at the start
        hideCursor();                  // hide blinking cursor
        srand(time(nullptr));          // init random

    ios_base::sync_with_stdio(false);  // disable sync with C stdio
    cin.tie(nullptr);                  // untie cin from cout
    cout.tie(nullptr);                 // output optimization
    this_thread::sleep_for(100ms); // small delay before the start screen appears

    // start screen
    cout << "Game ''The Stars Are Falling!''" << endl << "Press arrow keys or W/A/S/D (ENGLISH LAYOUT ONLY) to move and catch stars (Xx*+)" << endl;
    cout << "With each caught star the game speeds up!" << endl << "If you miss a star - you lose one life" << endl;
    cout << "Collect bonuses (h - health) to slow down the game and get extra lives." << "If a bonus falls - nothing happens!" << endl;
    this_thread::sleep_for(100ms);
    cout << "Press any key to continue..." << endl;
    _getch();

    // clear screen and update time variables
    system(CLEAR);                
    Clock::time_point lastMoveTime = Clock::now();   // last frame time
    Clock::time_point lastSpawnSTime = Clock::now(); // last star spawn time
    Clock::time_point lastSpawnBTime = Clock::now(); // last bonus spawn time

    // draw first frame and spawn first star/bonus
    draw(map, stars);
    spawnStar(stars);
    spawnBonus(bonuses);

    // main game loop
    while(gameRunning)
    {  
        auto currentFrame = Clock::now();
        auto frameDuration = chrono::duration_cast<chrono::milliseconds>(currentFrame - lastFrameTime).count();

        if (frameDuration < 16) // 60 fps max
        {
            this_thread::sleep_for(chrono::milliseconds(16 - frameDuration));
        }

        lastFrameTime = Clock::now();

        // clear each frame to draw a new one
        gotoxy(0, 0);

        // input for arrows and W/A/S/D keys
        if (_kbhit()) 
        {
            int key = _getch();
            
            // Handle special keys (arrows)
            if (key == 224) 
            {
                if (_kbhit())  
                {
                    key = _getch();
                    if (key == 75) playerX = max(0, playerX - 1);
                    if (key == 77) playerX = min(9, playerX + 1);
                    if (key == 72) playerY = max(0, playerY - 1);
                    if (key == 80) playerY = min(9, playerY + 1);
                }
            }
            // Handle regular letters w a s d
            else if (key == 'a' || key == 'A') playerX = max(0, playerX - 1);
            else if (key == 'd' || key == 'D') playerX = min(9, playerX + 1);
            else if (key == 'w' || key == 'W') playerY = max(0, playerY - 1);
            else if (key == 's' || key == 'S') playerY = min(9, playerY + 1);
            
            
        }
        // check real time
        auto currentTime = Clock::now();

        // movement + collision check
        auto elapsedMove = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastMoveTime).count();
        if (elapsedMove >= move_delay_ms) 
        {
            for (size_t i = 0; i < stars.size(); i++) 
            {
                stars[i].y++;
        
                if (stars[i].y >= playerY && stars[i].y <= playerY + 1 && stars[i].x == playerX) 
                {
                    score++;
                    stars[i].active = false;
                    move_delay_ms -= 5;
                    spawn_star_ms -= 5;
                }

                else if (stars[i].y >= groundY) 
                {
                    lives--;
                    stars[i].active = false;
                    move_delay_ms++;
                    spawn_star_ms++;
                }
            }

            // Bonus movement
            for (size_t i = 0; i < bonuses.size(); i++) 
            {
                bonuses[i].y++;
                if (bonuses[i].y >= playerY && bonuses[i].y <= playerY + 1 && bonuses[i].x == playerX) 
                {
                    lives++;
                    bonuses[i].active = false;
                    move_delay_ms += 10;
                    spawn_star_ms += 10;
                }

                else if (bonuses[i].y >= groundY) 
                {
                    bonuses[i].active = false;
                }
            
            }
            this_thread::sleep_for(10ms);
            lastMoveTime = currentTime;
        }

        // spawn stars
        auto elapsedSpawnS = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastSpawnSTime).count();
        if (elapsedSpawnS >= spawn_star_ms) 
        {
            spawnStar(stars);
            lastSpawnSTime = currentTime;
        }

        // spawn bonuses
        auto elapsedSpawnB = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastSpawnBTime).count();
        if (elapsedSpawnB >= spawn_bonus_ms) 
        {
            spawnBonus(bonuses);
            lastSpawnBTime = currentTime;
        }

        // remove old stars/bonuses
        for (int i = stars.size()-1; i >= 0; i--) if (stars[i].active == false) stars.erase(stars.begin() + i);
        for (int i = bonuses.size()-1; i >= 0; i--) if (bonuses[i].active == false) bonuses.erase(bonuses.begin() + i);

        // save high score
        if (score > highScore) highScore = score;

        // check score and loss
        if (lives <= 0) 
        {
            GameOver();
            gameRunning = false;
            continue;
        }



        draw(map, stars);
    }
    return 1;
}