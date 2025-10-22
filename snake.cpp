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
