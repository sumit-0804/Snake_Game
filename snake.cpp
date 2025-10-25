#include "snake.h"

deque<pair<int, int>> snake;

const int ROWS = 10;
const int COLS = 20;

void initSnake() {
    int midRow = ROWS / 2;
    int midCol = COLS / 2;
    for(int i =0; i<3; i++){
        snake.push_back({midRow, midCol - i});
    }
}

void drawSnake(vector<vector<char>>& board) {
    for (auto segment : snake) {
        int r = segment.first;
        int c = segment.second;
        board[r][c] = 'O';
    }
}

enum Direction { UP, DOWN, LEFT, RIGHT };
Direction dir = RIGHT;

void moveSnake() {
    pair<int, int> head = snake.front();
    switch (dir) {
        case UP: head.first--; break;
        case DOWN: head.first++; break;
        case LEFT: head.second--; break;
        case RIGHT: head.second++; break;
    }

    snake.push_front(head);
    snake.pop_back(); // remove tail (no growth yet)
}

bool checkCollision() {
    pair<int, int> head = snake.front();
    // Boundary collision
    if (head.first < 0 || head.first >= ROWS || head.second < 0 || head.second >= COLS)
        return true;
    // Self-collision
    for (int i = 1; i < snake.size(); i++) {
        if (snake[i] == head)
            return true;
    }
    return false;
}
