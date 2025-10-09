#include <iostream>
#include "board.h"
#include "snake.h"
using namespace std;
const int ROWS = 20;
const int COLS = 40;

void printBoard() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            bool printed = false;
            for (const auto& segment : snake) {
                if (segment.first == r && segment.second == c) {
                    cout << "🟩";  // snake body emoji
                    printed = true;
                    break;
                }
            }
            if(!printed) cout << "⬛";
        }
        cout << endl;
    }
}