#include <iostream> 
// ввод\вывод

#include <thread> 
/*сложная команда задержки времени - std::this_thread::sleep_for(std::chrono::milliseconds(ms))
но сокращается до this_thread::sleep_for(100ms);*/

#include <cstdlib> 
// для рандома букв врод, а также для очищения терминала при проигрыше

#include <vector>
// для списков(карты)

#include <conio.h>
// для неблокирующего ввода

#include <chrono>

#include <algorithm>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

using namespace std;
using namespace std::chrono_literals;

struct Star 
{
    int x, y = 0;
    char symbol = "*+•Xx"[rand() % 5];;
    bool active = true;   // не упала ли уже
};

// переменные для времени и самой игры
vector<Star> stars;   // буква, которая падает

int score = 0;
int groundY = 10;         // нижняя граница (если буква достигла этой строки - проигрыш)

int playerX = 5;
int playerY = groundY - 1;

bool gameRunning = true;  // флаг: игра продолжается или закончена

const int MOVE_DELAY_MS = 300; // время между кадрами
const int SPAWN_STAR_MS = 1500; // время появления новой звезды
using Clock = std::chrono::steady_clock;

vector<vector<char>> map(10, vector<char>(10, ' '));

// Функция рисует игровое поле на экране
void draw(vector<vector<char>>& map, vector<Star>& stars) 
{
    for (int i = 0; i < 10; i++) 
    {
        for (int j = 0; j < 10; j++)
        {
            map[i][j] = ' ';
        }
    }
    
    for (const auto& s : stars) 
    {
        if (s.active && s.y >= 0 && s.y < 10 && s.x >= 0 && s.x < 10) map[s.y][s.x] = s.symbol;
    }

    map[playerY][playerX] = 'V';

    // Перебираем все строки (i от 0 до 9)
    for (int i = 0; i < 10; i++) 
    {
        // Перебираем все символы в текущей строке (j от 0 до 9)
        for (int j = 0; j < 10; j++) 
        {
            cout << map[i][j];   // печатаем символ в клетке (i, j)
        }
        cout << endl;       // после строки переходим на новую линию
    }

    cout << "Ваш счет: " << score << endl;
    cout << "Кол-во звезд на поле: " << stars.size() << endl;
}

void spawnStar(vector<Star>& stars) {
    Star newStar;
    newStar.x = rand() % 10;
    newStar.y = 0;
    newStar.symbol = "*+Xx"[rand() % 4]; // разные символы
    newStar.active = true;
    stars.push_back(newStar);
}

void GameOver()
{
    system(CLEAR);
    cout << "Вы проиграли!" << endl << "Ваш счет:" << score << endl;
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    _getch();
}


// основная функция игры
int main()
{   
    system("chcp 65001 > nul"); // переключаем консоль в UTF-8
    system(CLEAR);
    srand(time(nullptr));
    this_thread::sleep_for(100ms);
    cout << "Игра ''The Stars Are Falling!''" << endl << "Нажимайте на стрелочки(←/→), чтобы двигаться и ловить звезды" << endl;
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    _getch();

    system(CLEAR);                    // чистим стартовый экран
    Clock::time_point lastMoveTime = Clock::now();
    Clock::time_point lastSpawnTime = Clock::now();

    draw(map, stars);
    spawnStar(stars);

    while(gameRunning)
    {  
        system(CLEAR);
        if (_kbhit()) 
        {
            int key = _getch();
            if (key == 224) 
            {
                key = _getch();
                if (key == 75) playerX = max(0, playerX-1);
                if (key == 77) playerX = min(9, playerX+1);
            }
        }

        auto currentTime = Clock::now();

        // Движение + проверка
        auto elapsedMove = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastMoveTime).count();
        if (elapsedMove >= MOVE_DELAY_MS) 
        {
            for (size_t i = 0; i < stars.size(); i++) 
            {
                stars[i].y++;   // двигаем вниз
        
                if (stars[i].y >= playerY && stars[i].y <= playerY + 1 && stars[i].x == playerX) score++, stars[i].active = false;

                else if (stars[i].y >= groundY) score--, stars[i].active = false;
        
            
            }
            
            lastMoveTime = currentTime;
        }
        
        auto elapsedSpawn = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastSpawnTime).count();
        if (elapsedSpawn >= SPAWN_STAR_MS) 
        {
            spawnStar(stars);
            lastSpawnTime = currentTime;
        }


        for (int i = stars.size()-1; i >= 0; i--) if (stars[i].active == false) stars.erase(stars.begin() + i);

        if (score <= -1) 
        {
            cout << "WIP" << endl;
            this_thread::sleep_for(500ms);
            GameOver();
            gameRunning = false;
            continue;
        }

        draw(map, stars);

        this_thread::sleep_for(80ms);
    }
    this_thread::sleep_for(500ms);
    return 1;
}