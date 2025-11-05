#include <iostream>
#include <limits>
#include <thread>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "ui.hpp"
#include "render.hpp"

using namespace std;

#ifdef _WIN32
static inline void ensureUtf8Console()
{
    static bool initialized = false;
    if (!initialized)
    {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        initialized = true;
    }
}
#endif

void clearScreen()
{
#ifdef _WIN32
    ensureUtf8Console();
#endif
    cout << clear() << gotorc(1, 1) << flush;
}

int showMainMenu()
{
    vector<string> options = {"Start Game", "View Top Scores", "Exit"};
    int selected = 0;
    int startRow = 10;
    int startCol = 15;

#ifdef _WIN32
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

    while (true)
    {
        for (int i = 0; i < (int)options.size(); ++i)
        {
            cout << gotorc(startRow + i, startCol);
            if (i == selected)
            {
                cout << Color::BRIGHT_GREEN << "> " << options[i] << Color::RESET;
            }
            else
            {
                cout << "  " << options[i];
            }
            cout << "\n";
        }
        cout << gotorc(startRow + (int)options.size() + 1, startCol)
             << "Use ↑/↓ or W/S to navigate, Enter to select." << endl;
        cout.flush();

#ifdef _WIN32
        while (!_kbhit())
            this_thread::sleep_for(chrono::milliseconds(10));
        int ch = _getch();
        if (ch == 224)
        {
            ch = _getch();
            if (ch == 72)
            {
                selected = (selected + (int)options.size() - 1) % options.size();
            }
            else if (ch == 80)
            {
                selected = (selected + 1) % options.size();
            }
        }
        else if (ch == 13)
        {
            break;
        }
        else if (ch == 'w' || ch == 'W')
        {
            selected = (selected + (int)options.size() - 1) % options.size();
        }
        else if (ch == 's' || ch == 'S')
        {
            selected = (selected + 1) % options.size();
        }
#else
        char ch = 0;
        ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n == 1)
        {
            if (ch == '\x1b')
            {
                char seq[2];
                if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1)
                {
                    if (seq[0] == '[')
                    {
                        if (seq[1] == 'A')
                        {
                            selected = (selected + (int)options.size() - 1) % options.size();
                        }
                        else if (seq[1] == 'B')
                        {
                            selected = (selected + 1) % options.size();
                        }
                    }
                }
            }
            else if (ch == '\n' || ch == '\r')
            {
                break;
            }
            else if (ch == 'w' || ch == 'W')
            {
                selected = (selected + (int)options.size() - 1) % options.size();
            }
            else if (ch == 's' || ch == 'S')
            {
                selected = (selected + 1) % options.size();
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
#endif
    }

#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    return selected + 1;
}

void showTopScores(const vector<PlayerScore> &scores)
{
    clearScreen();
    cout << Color::BRIGHT_YELLOW << Color::BOLD << "=== TOP SCORES ===\n\n"
         << Color::RESET;
    if (scores.empty())
    {
        cout << "No scores yet.\n\n";
    }
    else
    {
        int rank = 1;
        for (const auto &ps : scores)
        {
            cout << rank << ". " << ps.name << " - " << ps.score << "\n";
            rank++;
            if (rank > 10)
                break;
        }
        cout << "\n";
    }
    cout << "Press Enter to return to menu...";
    cout.flush();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string askPlayerName()
{
    clearScreen();

    int boxTop = 8;
    int boxLeft = 20;
    int boxWidth = 44;
    int boxHeight = 7;

    cout << gotorc(boxTop - 1, boxLeft)
         << Color::BRIGHT_YELLOW << Color::BOLD << u8"╔";
    for (int i = 0; i < boxWidth; ++i)
        cout << u8"═";
    cout << u8"╗" << Color::RESET;
    for (int r = 0; r < boxHeight; ++r)
    {
        cout << gotorc(boxTop + r, boxLeft)
             << Color::BRIGHT_YELLOW << Color::BOLD << u8"║" << Color::RESET
             << string(boxWidth, ' ')
             << Color::BRIGHT_YELLOW << Color::BOLD << u8"║" << Color::RESET;
    }
    cout << gotorc(boxTop + boxHeight, boxLeft)
         << Color::BRIGHT_YELLOW << Color::BOLD << u8"╚";
    for (int i = 0; i < boxWidth; ++i)
        cout << u8"═";
    cout << u8"╝" << Color::RESET;

    string title = " Enter Your Name ";
    int titleCol = boxLeft + (boxWidth - (int)title.size()) / 2 + 1;
    cout << gotorc(boxTop - 1, titleCol)
         << Color::BRIGHT_MAGENTA << title << Color::RESET;

    string prompt = "(max 15 chars)";
    int promptCol = boxLeft + (boxWidth - (int)prompt.size()) / 2 + 1;
    cout << gotorc(boxTop + 1, promptCol)
         << Color::DIM << prompt << Color::RESET;

    cout << gotorc(boxTop + 3, boxLeft + 3)
         << Color::BRIGHT_WHITE << "> " << Color::RESET;

    cout.flush();
    string name;
    cout << gotorc(boxTop + 3, boxLeft + 6);
    getline(cin, name);
    if (name.empty())
        name = "Player";
    if (name.size() > 15)
        name.resize(15);
    return name;
}

int showTitleAndMenu()
{
    cout << clear() << hideCursor();
    vector<string> title = {
        "  ███████╗███╗   ██╗ █████╗ ██╗  ██╗███████╗",
        "  ██╔════╝████╗  ██║██╔══██╗██║ ██╔╝██╔════╝",
        "  ███████╗██╔██╗ ██║███████║█████╔╝ █████╗  ",
        "  ╚════██║██║╚██╗██║██╔══██║██╔═██╗ ██╔══╝  ",
        "  ███████║██║ ╚████║██║  ██║██║  ██╗███████╗",
        "  ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝"};
    int startRow = 4;
    int startCol = 10;
    int screenRows = 22;
    int screenCols = 42;
    drawScreenBorder(screenRows, screenCols, startCol - 2, startRow - 2);
    vector<string> colors = {
        Color::BRIGHT_GREEN, Color::GREEN, Color::YELLOW,
        Color::BRIGHT_YELLOW, Color::BRIGHT_CYAN, Color::CYAN};
    for (size_t i = 0; i < title.size(); ++i)
    {
        cout << gotorc(startRow + (int)i, startCol)
             << colors[i] << Color::BOLD << title[i] << Color::RESET;
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    int subtitleRow = startRow + (int)title.size() + 2;
    cout << gotorc(subtitleRow, startCol + 13)
         << Color::BRIGHT_MAGENTA << "Console Edition 🎮" << Color::RESET;
    vector<string> options = {"Start Game", "View Top Scores", "Exit"};
    int selected = 0;
    int menuStartRow = subtitleRow + 3;

#ifdef _WIN32
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

    while (true)
    {
        for (int i = 0; i < (int)options.size(); ++i)
        {
            cout << gotorc(menuStartRow + i, startCol + 10);
            if (i == selected)
            {
                cout << Color::BRIGHT_GREEN << "> " << options[i] << Color::RESET;
            }
            else
            {
                cout << "  " << options[i];
            }
            cout << "\n";
        }
        cout << gotorc(menuStartRow + (int)options.size() + 2, startCol + 3)
             << Color::DIM << "Use ↑/↓ or W/S to navigate, Enter to select." << Color::RESET << endl;
        cout.flush();

#ifdef _WIN32
        while (!_kbhit())
            this_thread::sleep_for(chrono::milliseconds(10));
        int ch = _getch();
        if (ch == 224)
        {
            ch = _getch();
            if (ch == 72)
                selected = (selected + (int)options.size() - 1) % options.size();
            else if (ch == 80)
                selected = (selected + 1) % options.size();
        }
        else if (ch == 13)
        {
            break;
        }
        else if (ch == 'w' || ch == 'W')
        {
            selected = (selected + (int)options.size() - 1) % options.size();
        }
        else if (ch == 's' || ch == 'S')
        {
            selected = (selected + 1) % options.size();
        }
#else
        char ch = 0;
        ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n == 1)
        {
            if (ch == '\x1b')
            {
                char seq[2];
                if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1)
                {
                    if (seq[0] == '[')
                    {
                        if (seq[1] == 'A')
                            selected = (selected + (int)options.size() - 1) % options.size();
                        else if (seq[1] == 'B')
                            selected = (selected + 1) % options.size();
                    }
                }
            }
            else if (ch == '\n' || ch == '\r')
            {
                break;
            }
            else if (ch == 'w' || ch == 'W')
            {
                selected = (selected + (int)options.size() - 1) % options.size();
            }
            else if (ch == 's' || ch == 'S')
            {
                selected = (selected + 1) % options.size();
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
#endif
    }

#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    return selected + 1;
}
