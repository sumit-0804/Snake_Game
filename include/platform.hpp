#pragma once

#include <utility>

// Windows VT/UTF-8 enabling (no-op on non-Windows)
void enableVTAndUTF8();

// Get terminal rows, cols
std::pair<int, int> getTerminalSize();
