#include <iostream>
#include <vector>
#include <algorithm>
#include "types.hpp"
#include "platform.hpp"
#include "util.hpp"
#include "ui.hpp"
#include "render.hpp"
#include "game.hpp"
#include "score.hpp"

using namespace std;

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

#ifdef _WIN32
    enableVTAndUTF8();
#endif

    string scoresPath = getDefaultScoresPath();
    vector<PlayerScore> topScores = loadScores(scoresPath);
    int originRow = 5;
    int originCol = 9;
    bool running = true;

    while (running)
    {
        int choice = showTitleAndMenu();
        if (choice == 1)
        {
            GameConfig cfg;
            calculateBoardSize(cfg);
            GameState st;
            initGameState(cfg, st);
            string playerName = askPlayerName();
            runGame(cfg, st, originRow, originCol);
            if (!st.quit)
            {
                addScore(topScores, playerName, st.score);
                saveScores(scoresPath, topScores);
                bool restart = showGameOverScreen(cfg, originRow, originCol, st, topScores);
                if (!restart)
                    running = false;
            }
            else
            {
                running = false;
            }
        }
        else if (choice == 2)
        {
            showTopScores(topScores);
        }
        else if (choice == 3)
        {
            running = false;
        }
    }
    cout << showCursor() << clear() << gotorc(1, 1);
    cout << Color::BRIGHT_CYAN << "Thanks for playing! 👋" << Color::RESET << "\n\n";
    cout.flush();
    return 0;
}
