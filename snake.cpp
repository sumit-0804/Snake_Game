#include <iostream>
#include <deque>
#include <algorithm>
#include <chrono>
#include <conio.h>
#include <windows.h>
#include <random>

using namespace std;

const int WIDTH = 30;
const int HEIGHT = 20;
const int GAME_SPEED = 150;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct Position {
    int x, y;
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

deque<Position> snake;
Position food;
Direction dir = STOP;
bool gameOver = false;
int score = 0;

void SetCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void SetupConsole() {
    SetConsoleOutputCP(CP_UTF8);
    HideCursor();
}

void GenerateFood() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> disX(1, WIDTH - 2);
    uniform_int_distribution<> disY(1, HEIGHT - 2);
    
    do {
        food.x = disX(gen);
        food.y = disY(gen);
    } while (find(snake.begin(), snake.end(), food) != snake.end());
}

void Setup() {
    SetupConsole();
    snake.clear();
    snake.push_back({WIDTH / 2, HEIGHT / 2});
    snake.push_back({WIDTH / 2 - 1, HEIGHT / 2});
    snake.push_back({WIDTH / 2 - 2, HEIGHT / 2});
    GenerateFood();
    dir = RIGHT;
    score = 0;
    gameOver = false;
}

void Draw() {
    SetCursorPosition(0, 0);
    for (int i = 0; i < WIDTH; i++) cout << "🟥";
    cout << "    SCORE: " << score;
    
    for (int i = 1; i < HEIGHT - 1; i++) {
        SetCursorPosition(0, i);
        cout << "🟥";
        
        for (int j = 1; j < WIDTH - 1; j++) {
            Position current = {j, i};
            
            if (current == snake.front()) {
                cout << "🐍";
            }
            else if (find(snake.begin() + 1, snake.end(), current) != snake.end()) {
                cout << "🐍";
            }
            else if (current == food) {
                cout << "🍕";
            }
            else {
                cout << "🔳";
            }
        }
        
        cout << "🟥";
    }
    
    SetCursorPosition(0, HEIGHT - 1);
    for (int i = 0; i < WIDTH; i++) cout << "🟥";
}

void Input() {
    if (_kbhit()) {
        char key = _getch();
        
        switch (key) {
            case 'w':
            case 'W':
            case 72:
                if (dir != DOWN) dir = UP;
                break;
            case 's':
            case 'S':
            case 80:
                if (dir != UP) dir = DOWN;
                break;
            case 'a':
            case 'A':
            case 75:
                if (dir != RIGHT) dir = LEFT;
                break;
            case 'd':
            case 'D':
            case 77:
                if (dir != LEFT) dir = RIGHT;
                break;
            case 'q':
            case 'Q':
            case 'x':
            case 'X':
            case 27: // ESC key
                gameOver = true;
                break;
        }
    }
}


void Logic() {
    if (dir == STOP) return;
    
    Position newHead = snake.front();
    switch (dir) {
        case LEFT:  newHead.x--; break;
        case RIGHT: newHead.x++; break;
        case UP:    newHead.y--; break;
        case DOWN:  newHead.y++; break;
        default: break;
    }
    
    if (newHead.x <= 0 || newHead.x >= WIDTH - 1 || 
        newHead.y <= 0 || newHead.y >= HEIGHT - 1) {
        gameOver = true;
        return;
    }
    
    if (find(snake.begin(), snake.end(), newHead) != snake.end()) {
        gameOver = true;
        return;
    }
    
    snake.push_front(newHead);
    
    if (newHead == food) {
        score += 10;
        GenerateFood();
    } else {
        snake.pop_back();
    }
}

int main() {
    Setup();
    Draw();
    
    auto lastUpdate = chrono::steady_clock::now();
    
    while (!gameOver) {
        Input();
        
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - lastUpdate).count();
        
        if (elapsed >= GAME_SPEED) {
            Logic();
            Draw();
            lastUpdate = now;
        }
        
        Sleep(10);
    }
    
    SetCursorPosition(0, HEIGHT + 1);
    cout << "\n╔════════════════════════════════════╗\n";
    cout << "║        🎮 GAME OVER! 🎮          ║\n";
    cout << "║                                    ║\n";
    cout << "║      Final Score: " << score;
    if (score < 100) cout << " ";
    if (score < 10) cout << " ";
    cout << "             ║\n";
    cout << "╚════════════════════════════════════╝\n";
    
    return 0;
}
