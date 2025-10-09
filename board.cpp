#include <iostream>
#include "board.h"
#include "snake.h"
using namespace std;
const int ROWS = 10;
const int COLS = 20;
void printBoard() {
    for (int c = 0; c < COLS + 2; c++) cout << "🟥";
    cout << "\n";

    for (int r = 0; r < ROWS; r++) {
        cout << "🟥";
        for (int c = 0; c < COLS; c++) {
            bool printed = false;
            for (size_t i = 0; i < snake.size(); i++) {
                if (snake[i].first == r && snake[i].second == c) {
                    cout << (i == 0 ? "🐍" : "🟢");
                    printed = true;
                    break;
                }
            }
            if (!printed) cout << "⬛";
        }
        cout << "🟥\n";
    }

    for (int c = 0; c < COLS + 2; c++) cout << "🟥";
    cout << "\n";
}
