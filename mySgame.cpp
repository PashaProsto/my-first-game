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
#include <windows.h>  // нужно для gotoxy
#endif

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

// Функция перемещения курсора в позицию (0,0) - левый верхний угол
// На Windows и Linux работает по-разному, поэтому используем условную компиляцию
void gotoxy(int x, int y) 
{
#ifdef _WIN32
    // Windows: используем SetConsoleCursorPosition
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    // Linux/Mac: используем ANSI escape-последовательности
    // \033[ - начало escape-последовательности
    // y;xH - перемещает курсор на строку y, колонку x
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

// Функция скрытия курсора (чтобы не мигал)
void hideCursor() 
{
#ifdef _WIN32
    // Windows: прячем консольный курсор
    CONSOLE_CURSOR_INFO cursorInfo;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo); 
#else
    // Linux: ANSI код для скрытия курсора
    printf("\033[?25l");
#endif
}

using namespace std;
using namespace std::chrono_literals;

// класс звезды
struct Star 
{
    int x, y = 0;
    char symbol = "*+Xx"[rand() % 4];;
    bool active = true;   // не упала ли уже
    int type = 0; // 0=обычная звезда, 1=дополнительная жизнь
};

// класс доп. жизни
struct Bonus 
{
    int x, y = 0;
    char symbol = 'H';
    bool active = true;
};

// переменные для времени и самой игры
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
int move_delay_ms = 200;   // время между кадрами
int spawn_star_ms = 1000;  // время между появлениями новой звезды
int spawn_bonus_ms = 3000; // время между появлениями бонуса
auto lastFrameTime = Clock::now();

vector<vector<char>> map(10, vector<char>(10, ' '));

// Функция рисует игровое поле на экране
void draw(vector<vector<char>>& map, vector<Star>& stars) 
{
    // рисуем игровое поле
    for (int i = 0; i < 10; i++) 
    {
        for (int j = 0; j < 10; j++)
        {
            map[i][j] = ' ';
        }
    }
    
    // рисуем падающие звезды
    for (const auto& s : stars) 
    {
        if (s.active && s.y >= 0 && s.y < 10 && s.x >= 0 && s.x < 10) map[s.y][s.x] = s.symbol;
    }

    for (const auto& b : bonuses) 
    {
        if (b.active && b.y >= 0 && b.y < 10 && b.x >= 0 && b.x < 10) map[b.y][b.x] = b.symbol;
    }

    // рисуем игрока
    map[playerY][playerX] = 'V';

    // печатаем все, что есть на карте
    for (int i = 0; i < 10; i++) 
    {
        
        for (int j = 0; j < 10; j++) 
        {
            cout << map[i][j];
        }
        cout << endl;
    }

    cout << "Ваш счет: " << score << endl;
    cout << "Ваши жизни: " << lives << endl;
    cout << "Скорость игры: " << 1000 / move_delay_ms + 95 << "%" << endl;
}


// функция спавна новой звезды
void spawnStar(vector<Star>& stars) {
    Star newStar;
    newStar.x = rand() % 10;
    newStar.y = 0;
    newStar.symbol = "*+Xx"[rand() % 4]; // разные символы
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
    cout << "Вы проиграли!" << endl << "Ваш наилучший счет:" << highScore << endl;
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    this_thread::sleep_for(500ms);
    _getch();
}


// основная функция игры
int main()
{   
    #ifdef _WIN32
        system("chcp 65001 > nul");    // Windows: переключаем консоль в UTF-8
    #endif
        system(CLEAR);                 // очищаем экран один раз в начале
        hideCursor();                  // скрываем мигающий курсор
        srand(time(nullptr));          // инициализация рандома

    ios_base::sync_with_stdio(false);  // отключаем синхронизацию с C stdio
    cin.tie(nullptr);                  // отвязываем cin от cout
    cout.tie(nullptr);                 // оптимизация вывода
    this_thread::sleep_for(100ms); // небольшая задержка перед появлением стратового экрана

    // стартовый экран
    cout << "Игра ''The Stars Are Falling!''" << endl << "Нажимайте на стрелочки или же на W/A/S/D(ТОЛЬКО АНГЛИЙСКАЯ РАСКЛАДКА), чтобы двигаться и ловить звезды(Xx*+)" << endl;
    cout << "С каждой пойманной звездой игра ускоряется!" << endl << "Если вы не словите звезду - то у вас отнимется одна жизнь" << endl;
    cout << "Собирайте бонусы(h - health) чтобы замедлить игру и получить доп. жизни." << "Если бонус упадет - ничего не случится!" << endl;
    this_thread::sleep_for(100ms);
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    _getch();

    // очистка экрана и обновления переменных времени
    system(CLEAR);                
    Clock::time_point lastMoveTime = Clock::now();   // последнее время кадра
    Clock::time_point lastSpawnSTime = Clock::now(); // последнее время спавна звезды
    Clock::time_point lastSpawnBTime = Clock::now(); // последнее время спавна бонуса

    // отрисовка первого кадра и спавн первой звезды/бонуса
    draw(map, stars);
    spawnStar(stars);
    spawnBonus(bonuses);

    // основной игровой цикл
    while(gameRunning)
    {  
        auto currentFrame = Clock::now();
        auto frameDuration = chrono::duration_cast<chrono::milliseconds>(currentFrame - lastFrameTime).count();

        if (frameDuration < 16) // максимум 60 фпс
        {
            this_thread::sleep_for(chrono::milliseconds(16 - frameDuration));
        }

        lastFrameTime = Clock::now();

        // очистка каждого кадра для отрисовки нового
        gotoxy(0, 0);

        // ввод на стрелочки и буквы A/D
        if (_kbhit()) 
        {
            int key = _getch();
            
            // Обработка специальных клавиш (стрелки)
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
            // Обработка обычных букв A и D
            else if (key == 'a' || key == 'A') playerX = max(0, playerX - 1);
            else if (key == 'd' || key == 'D') playerX = min(9, playerX + 1);
            else if (key == 'w' || key == 'W') playerY = max(0, playerY - 1);
            else if (key == 's' || key == 'S') playerY = min(9, playerY + 1);
            
            
        }
        // проверка настоящего времени
        auto currentTime = Clock::now();

        // движение + проверка столкновения
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

            // Движение бонусов
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

        // спавн звезд
        auto elapsedSpawnS = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastSpawnSTime).count();
        if (elapsedSpawnS >= spawn_star_ms) 
        {
            spawnStar(stars);
            lastSpawnSTime = currentTime;
        }

        // спавн бонусов
        auto elapsedSpawnB = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastSpawnBTime).count();
        if (elapsedSpawnB >= spawn_bonus_ms) 
        {
            spawnBonus(bonuses);
            lastSpawnBTime = currentTime;
        }

        // удаление старых звезд/бонусов
        for (int i = stars.size()-1; i >= 0; i--) if (stars[i].active == false) stars.erase(stars.begin() + i);
        for (int i = bonuses.size()-1; i >= 0; i--) if (bonuses[i].active == false) bonuses.erase(bonuses.begin() + i);

        // сохранение максимального рекорда
        if (score > highScore) highScore = score;

        // проверка счета и проигрыш
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