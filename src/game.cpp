#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include "game.hpp"
#include "render.hpp"
#include "util.hpp"
#include "score.hpp"

using namespace std;

static atomic<Dir> inputDir;
static atomic<bool> inputQuit{false};

static void inputThreadFunc()
{
#ifdef _WIN32
    while (!inputQuit.load())
    {
        if (_kbhit())
        {
            int ch = _getch();
            if (ch == 'q' || ch == 'Q')
            {
                inputQuit = true;
                break;
            }
            if (ch == 224)
            {
                int ar = _getch();
                if (ar == 72)
                    inputDir = UP;
                else if (ar == 80)
                    inputDir = DOWN;
                else if (ar == 75)
                    inputDir = LEFT;
                else if (ar == 77)
                    inputDir = RIGHT;
            }
            else
            {
                if (ch == 'w' || ch == 'W')
                    inputDir = UP;
                else if (ch == 's' || ch == 'S')
                    inputDir = DOWN;
                else if (ch == 'a' || ch == 'A')
                    inputDir = LEFT;
                else if (ch == 'd' || ch == 'D')
                    inputDir = RIGHT;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(1));
    }
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    while (!inputQuit.load())
    {
        char ch;
        ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n == 1)
        {
            if (ch == 'q' || ch == 'Q')
            {
                inputQuit = true;
                break;
            }
            if (ch == '\x1b')
            {
                char seq[2];
                if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1)
                {
                    if (seq[0] == '[')
                    {
                        if (seq[1] == 'A')
                            inputDir = UP;
                        else if (seq[1] == 'B')
                            inputDir = DOWN;
                        else if (seq[1] == 'D')
                            inputDir = LEFT;
                        else if (seq[1] == 'C')
                            inputDir = RIGHT;
                    }
                }
            }
            else
            {
                if (ch == 'w' || ch == 'W')
                    inputDir = UP;
                else if (ch == 's' || ch == 'S')
                    inputDir = DOWN;
                else if (ch == 'a' || ch == 'A')
                    inputDir = LEFT;
                else if (ch == 'd' || ch == 'D')
                    inputDir = RIGHT;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    // Restore terminal settings and file descriptor flags
    fcntl(STDIN_FILENO, F_SETFL, flags);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

bool showGameOverScreen(const GameConfig &cfg, int originRow, int originCol, const GameState &st, const vector<PlayerScore> &topScores)
{
    int msgRow = originRow + cfg.rows / 2 - 7;
    int msgCol = originCol + cfg.cols - 12;

    cout << gotorc(msgRow, msgCol)
         << Color::bg(52) << Color::BRIGHT_WHITE << Color::BOLD
         << "                     " << Color::RESET;
    cout << gotorc(msgRow + 1, msgCol)
         << Color::bg(52) << Color::BRIGHT_WHITE << Color::BOLD
         << "   GAME OVER! 💀     " << Color::RESET;
    cout << gotorc(msgRow + 2, msgCol)
         << Color::bg(52) << Color::BRIGHT_WHITE << Color::BOLD
         << "                     " << Color::RESET;
    cout << gotorc(msgRow + 3, msgCol)
         << Color::bg(52) << Color::BRIGHT_YELLOW
         << "  Your Score: " << st.score << "      " << Color::RESET;
    cout << gotorc(msgRow + 4, msgCol)
         << Color::bg(52) << Color::BRIGHT_WHITE << Color::BOLD
         << "                     " << Color::RESET;

    int scoresRow = msgRow + 6;
    cout << gotorc(scoresRow, msgCol)
         << Color::bg(22) << Color::BRIGHT_WHITE << Color::BOLD
         << "   TOP 4 SCORES      " << Color::RESET;

    int displayCount = min(4, (int)topScores.size());
    for (int i = 0; i < displayCount; ++i)
    {
        string rank = to_string(i + 1) + ".";
        string name = topScores[i].name;
        if (name.length() > 10)
        {
            name = name.substr(0, 7) + "...";
        }
        string score = to_string(topScores[i].score);

        string scoreLine = "  " + rank;
        if (rank.length() == 2)
            scoreLine += " ";
        scoreLine += " " + name;

        int totalWidth = 21;
        int usedWidth = (int)scoreLine.length() + (int)score.length();
        if (usedWidth < totalWidth)
        {
            scoreLine += string(totalWidth - usedWidth, ' ') + score;
        }
        else
        {
            scoreLine += " " + score;
        }

        cout << gotorc(scoresRow + 1 + i, msgCol)
             << Color::bg(22) << Color::BRIGHT_YELLOW
             << scoreLine << Color::RESET;
    }

    for (int i = displayCount; i < 4; ++i)
    {
        string rank = to_string(i + 1) + ".";
        cout << gotorc(scoresRow + 1 + i, msgCol)
             << Color::bg(22) << Color::BRIGHT_YELLOW
             << "  " << rank;
        if (rank.length() == 2)
            cout << " ";
        cout << " ---" << string(15, ' ') << Color::RESET;
    }

    int promptRow = scoresRow + 6;
    cout << gotorc(promptRow, msgCol)
         << Color::bg(22) << Color::BRIGHT_WHITE
         << " Enter/WASD: Restart " << Color::RESET;
    cout << gotorc(promptRow + 1, msgCol)
         << Color::bg(88) << Color::BRIGHT_WHITE
         << " Q/E/Esc: Exit      " << Color::RESET;
    cout.flush();

#ifdef _WIN32
    while (!_kbhit())
        this_thread::sleep_for(chrono::milliseconds(50));
    int ch = _getch();

    if (ch == 'q' || ch == 'Q' || ch == 'e' || ch == 'E' || ch == 27)
    {
        return false;
    }
    if (ch == 224)
    {
        _getch();
        return true;
    }
    if (ch == 13 || ch == 'w' || ch == 'W' || ch == 'a' || ch == 'A' ||
        ch == 's' || ch == 'S' || ch == 'd' || ch == 'D')
    {
        return true;
    }
    return false;
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char ch = getchar();
    // If we got escape, check if it's standalone or part of a sequence
    if (ch == '\x1b')
    {
        // Temporarily enable non-blocking to check for sequence
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        char next;
        ssize_t n = read(STDIN_FILENO, &next, 1);
        fcntl(STDIN_FILENO, F_SETFL, flags);
        if (n <= 0)
        {
            // Standalone Escape - exit
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return false;
        }
        // It's part of a sequence (arrow key), consume it but treat as restart
        if (n == 1 && next == '[')
        {
            read(STDIN_FILENO, &next, 1); // Consume the direction character
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return true; // Arrow keys treated as restart
    }
    // Check for exit keys: q, Q, e, E
    if (ch == 'q' || ch == 'Q' || ch == 'e' || ch == 'E')
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return false;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    if (ch == '\n' || ch == 'w' || ch == 'W' || ch == 'a' || ch == 'A' ||
        ch == 's' || ch == 'S' || ch == 'd' || ch == 'D')
    {
        return true;
    }
    return false;
#endif
}

void initGameState(const GameConfig &cfg, GameState &st)
{
    st.snake.clear();
    st.obstacles.clear();
    st.gameOver = false;
    st.quit = false;
    st.score = 0;
    st.moves = 0;
    st.goldenApplesEaten = 0;
    st.segmentsToGrow = 0;
    st.dir = RIGHT;
    int sr = cfg.rows / 2, sc = cfg.cols / 2 - 2;
    st.snake.push_back({sr, sc + 2});
    st.snake.push_back({sr, sc + 1});
    st.snake.push_back({sr, sc});
    inputDir = st.dir;
    inputQuit = false;
    buildObstacles(cfg, st);
    placeFood(cfg, st);
}

void runGame(const GameConfig &cfg, GameState &st, int originRow, int originCol)
{
    drawBoardInitial(cfg, st, originRow, originCol);
    string foodGlyph = (st.foodType == GOLDEN) ? FOOD_GOLDEN : FOOD_NORMAL;
    drawCell(cfg, originRow, originCol, st.food, foodGlyph);

    for (size_t i = 1; i < st.snake.size(); i++)
        drawCell(cfg, originRow, originCol, st.snake[i], SNAKE_BODY);

    drawCell(cfg, originRow, originCol, st.snake.front(), SNAKE_HEAD);
    drawScorePanel(cfg, originRow, originCol, st);
    cout.flush();

    thread inputThread(inputThreadFunc);
    const int frame_ms = std::max(16, 1000 / cfg.fps);
    auto last = chrono::steady_clock::now();

    while (!st.gameOver && !st.quit)
    {
        Dir nd = inputDir.load();
        if (!isOpposite(st.dir, nd))
            st.dir = nd;
        if (inputQuit.load())
        {
            st.quit = true;
            break;
        }
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - last).count();
        if (elapsed < frame_ms)
        {
            this_thread::sleep_for(chrono::milliseconds(frame_ms - elapsed));
        }
        last = chrono::steady_clock::now();
        Vec newHead = step(st.snake.front(), st.dir);
        if (!inBounds(cfg, newHead))
            st.gameOver = true;
        if (!st.gameOver && st.obstacles.count({newHead.r, newHead.c}))
            st.gameOver = true;
        if (!st.gameOver)
        {
            for (auto &seg : st.snake)
                if (seg.r == newHead.r && seg.c == newHead.c)
                {
                    st.gameOver = true;
                    break;
                }
        }
        if (st.gameOver)
            break;
        Vec tail = st.snake.back();
        st.snake.push_front(newHead);
        st.moves++;
        bool ate = (newHead.r == st.food.r && newHead.c == st.food.c);
        if (ate)
        {
            if (st.foodType == GOLDEN)
            {
                st.score += cfg.goldenAppleScore;
                st.goldenApplesEaten++;
                st.segmentsToGrow += 2;
            }
            else
            {
                st.score += cfg.normalAppleScore;
                st.segmentsToGrow += 1;
            }
            placeFood(cfg, st);
        }
        if (st.segmentsToGrow > 0)
        {
            st.segmentsToGrow--;
        }
        else
        {
            st.snake.pop_back();
            drawCell(cfg, originRow, originCol, tail, EMPTY);
        }
        if (st.snake.size() > 1)
        {
            drawCell(cfg, originRow, originCol, st.snake[1], SNAKE_BODY);
        }
        drawCell(cfg, originRow, originCol, st.snake.front(), SNAKE_HEAD);
        if (ate && !st.gameOver)
        {
            foodGlyph = (st.foodType == GOLDEN) ? FOOD_GOLDEN : FOOD_NORMAL;
            drawCell(cfg, originRow, originCol, st.food, foodGlyph);
        }
        drawScorePanel(cfg, originRow, originCol, st);
        cout.flush();
    }
    inputQuit = true;
    if (inputThread.joinable())
        inputThread.join();
    // Small delay to ensure terminal state is fully restored
    this_thread::sleep_for(chrono::milliseconds(50));
#ifdef _WIN32
    // Flush any pending input on Windows
    while (_kbhit())
        _getch();
#else
    // Flush any pending input on Linux
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    char ch;
    while (read(STDIN_FILENO, &ch, 1) > 0)
    {
    } // Flush input buffer
    fcntl(STDIN_FILENO, F_SETFL, flags);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}
