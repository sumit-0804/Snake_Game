#include "score.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <filesystem>

using namespace std;

static string joinPath(const string &a, const string &b)
{
    if (a.empty())
        return b;
    char sep =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif
    if (a.back() == sep)
        return a + b;
    return a + sep + b;
}

string getDefaultScoresPath()
{
    string baseDir;
#ifdef _WIN32
    const char *appdata = getenv("APPDATA");
    if (appdata && *appdata)
    {
        baseDir = string(appdata);
        baseDir = joinPath(baseDir, "SnakeConsole");
    }
    else
    {
        const char *user = getenv("USERPROFILE");
        baseDir = user ? string(user) : string(".");
        baseDir = joinPath(baseDir, "SnakeConsole");
    }
#else
    const char *home = getenv("HOME");
    baseDir = home ? string(home) : string(".");
    baseDir = joinPath(baseDir, ".local");
    baseDir = joinPath(baseDir, "share");
    baseDir = joinPath(baseDir, "SnakeConsole");
#endif
    std::error_code ec;
    std::filesystem::create_directories(baseDir, ec);
    return joinPath(baseDir, "scores.txt");
}

static bool parseLine(const string &line, PlayerScore &out)
{
    if (line.empty())
        return false;
    size_t tab = line.rfind('\t');
    if (tab == string::npos)
        return false;
    string name = line.substr(0, tab);
    string s = line.substr(tab + 1);
    try
    {
        int sc = stoi(s);
        out.name = name;
        out.score = sc;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

vector<PlayerScore> loadScores(const string &path)
{
    vector<PlayerScore> res;
    ifstream in(path);
    if (!in)
        return res;
    string line;
    while (getline(in, line))
    {
        PlayerScore ps;
        if (parseLine(line, ps))
            res.push_back(ps);
    }
    sort(res.begin(), res.end(), [](const PlayerScore &a, const PlayerScore &b)
         { return b.score < a.score; });
    if (res.size() > 10)
        res.resize(10);
    return res;
}

bool saveScores(const string &path, const vector<PlayerScore> &scores)
{
    ofstream out(path, ios::trunc);
    if (!out)
        return false;
    for (const auto &ps : scores)
    {
        out << ps.name << '\t' << ps.score << '\n';
    }
    return true;
}

void addScore(vector<PlayerScore> &scores, const string &name, int score)
{
    scores.push_back({name, score});
    sort(scores.begin(), scores.end(), [](const PlayerScore &a, const PlayerScore &b)
         { return b.score < a.score; });
    if (scores.size() > 10)
        scores.resize(10);
}
