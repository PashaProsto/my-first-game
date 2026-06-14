# The Stars Are Falling!

A terminal arcade game where you catch falling stars and avoid missing them. Collect health bonuses to gain extra lives.

## Gameplay
- Control the player character (`V`) at the bottom of a 10x10 grid.
- Catch falling stars (`*`, `+`, `X`, `x`) to increase your score.
- Each caught star speeds up the game (reduces delay between moves).
- Missing a star costs one life and slows the game down.
- Collect hearts (`h`) to gain an extra life and temporarily slow the game.

## Controls
- **Arrow keys** or **W/A/S/D** (English layout) to move left/right/up/down.
- Press any key to start after the info screen.

## Building and Running
- Compile with a C++11 (or later) compiler.
- Windows: `g++ mySgame.cpp -o game.exe -std=c++11`
- Linux: install `libncurses` or compile without conio (the code uses `_kbhit`/`_getch` – Windows only). For Linux, replace conio with termios or use a cross‑platform library.
- Run the executable in a terminal.

## Requirements
- C++11 or higher.
- Windows recommended (uses `<conio.h>`). Linux support is partial (ANSI escape sequences for cursor, but input may need adaptation).

## Notes
- High score is tracked during the session.
- Game over when lives reach 0.

---

# The Stars Are Falling! (Russian)

Терминальная аркадная игра, в которой нужно ловить падающие звёзды и не пропускать их. Собирайте бонусы‑сердца для получения дополнительных жизней.

## Геймплей
- Управляйте персонажем (`V`) внизу игрового поля 10x10.
- Ловите падающие звёзды (`*`, `+`, `X`, `x`), чтобы увеличить счёт.
- Каждая пойманная звезда ускоряет игру (уменьшает задержку между ходами).
- Пропущенная звезда отнимает одну жизнь и замедляет игру.
- Бонусы (`h`) дают дополнительную жизнь и временно замедляют игру.

## Управление
- **Стрелки** или **W/A/S/D** (английская раскладка) для движения влево/вправо/вверх/вниз.
- Нажмите любую клавишу после стартового экрана.

## Сборка и запуск
- Компилятор с поддержкой C++11 или новее.
- Windows: `g++ mySgame.cpp -o game.exe -std=c++11`
- Linux: требуется адаптация ввода (код использует `<conio.h>`). Рекомендуется запуск на Windows.
- Запустите исполняемый файл в терминале.

## Требования
- C++11 или выше.
- Рекомендуется Windows (из‑за `<conio.h>`). Поддержка Linux частичная (ANSI‑коды для курсора работают, но ввод может потребовать доработки).

## Примечания
- Рекорд сохраняется в рамках одной сессии.
- Игра заканчивается, когда жизни достигают нуля.
