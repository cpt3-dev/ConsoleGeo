#include "ui_handler.hpp"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    // Enable virtual terminal processing for ANSI escape sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            dwMode |= DISABLE_NEWLINE_AUTO_RETURN;
            SetConsoleMode(hOut, dwMode);
        }
    }

    // Enable mouse input and disable quick edit mode
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hIn, &dwMode)) {
            dwMode |= ENABLE_MOUSE_INPUT;
            dwMode |= ENABLE_EXTENDED_FLAGS;
            dwMode &= ~ENABLE_QUICK_EDIT_MODE;
            dwMode &= ~ENABLE_PROCESSED_INPUT;
            SetConsoleMode(hIn, dwMode);
        }
    }

    // Set console output codepage to UTF-8 for Unicode block characters
    SetConsoleOutputCP(CP_UTF8);
#endif

    ConsoleGeo::UIHandler app;
    app.run();

    return 0;
}