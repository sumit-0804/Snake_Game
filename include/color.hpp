#pragma once

#include <string>

namespace Color
{
    inline const std::string RESET = "\x1b[0m";
    inline const std::string BOLD = "\x1b[1m";
    inline const std::string DIM = "\x1b[2m";
    inline const std::string UNDERLINE = "\x1b[4m";

    inline const std::string RED = "\x1b[31m";
    inline const std::string GREEN = "\x1b[32m";
    inline const std::string YELLOW = "\x1b[33m";
    inline const std::string BLUE = "\x1b[34m";
    inline const std::string MAGENTA = "\x1b[35m";
    inline const std::string CYAN = "\x1b[36m";
    inline const std::string WHITE = "\x1b[37m";

    inline const std::string BRIGHT_RED = "\x1b[91m";
    inline const std::string BRIGHT_GREEN = "\x1b[92m";
    inline const std::string BRIGHT_YELLOW = "\x1b[93m";
    inline const std::string BRIGHT_BLUE = "\x1b[94m";
    inline const std::string BRIGHT_MAGENTA = "\x1b[95m";
    inline const std::string BRIGHT_CYAN = "\x1b[96m";
    inline const std::string BRIGHT_WHITE = "\x1b[97m";

    inline std::string bg(int id) { return "\x1b[48;5;" + std::to_string(id) + "m"; }
    inline std::string fg(int id) { return "\x1b[38;5;" + std::to_string(id) + "m"; }
}
