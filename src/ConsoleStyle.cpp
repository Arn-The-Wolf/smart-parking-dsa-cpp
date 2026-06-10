#include "ConsoleStyle.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace ConsoleStyle {

namespace {

constexpr const char* kReset = "\033[0m";
constexpr const char* kBold = "\033[1m";
constexpr const char* kDim = "\033[2m";
constexpr const char* kCyan = "\033[36m";
constexpr const char* kGreen = "\033[32m";
constexpr const char* kYellow = "\033[33m";
constexpr const char* kMagenta = "\033[35m";
constexpr const char* kBlue = "\033[34m";
constexpr const char* kRed = "\033[31m";
constexpr const char* kWhite = "\033[97m";

}  // namespace

void enableColors() {
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE) {
        return;
    }
    DWORD mode = 0;
    if (GetConsoleMode(handle, &mode)) {
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(handle, mode);
    }
#endif
}

void reset() {
    std::cout << kReset;
}

void printBannerLine(char fill) {
    std::cout << kCyan << kBold;
    for (int i = 0; i < 40; ++i) {
        std::cout << fill;
    }
    std::cout << kReset << "\n";
}

void printTitle(const std::string& text) {
    std::cout << kCyan << kBold << text << kReset << "\n";
}

void printSubtitle(const std::string& text) {
    std::cout << kDim << text << kReset << "\n";
}

void printSection(const std::string& text) {
    std::cout << kYellow << kBold << "  " << text << kReset << "\n";
}

void printMenuItem(int option, const std::string& text) {
    if (option == 0) {
        std::cout << kRed << kBold << "    " << option << ".  " << kReset << kRed << text << kReset << "\n";
        return;
    }
    if (option >= 13) {
        std::cout << kMagenta << "   " << option << ".  " << kReset << text << "\n";
        return;
    }
    std::cout << kGreen << "    " << option << ".  " << kReset << text << "\n";
}

void printSuccess(const std::string& text) {
    std::cout << kGreen << text << kReset << "\n";
}

void printError(const std::string& text) {
    std::cout << kRed << text << kReset << "\n";
}

void printWarning(const std::string& text) {
    std::cout << kYellow << text << kReset << "\n";
}

void printInfo(const std::string& text) {
    std::cout << kBlue << text << kReset << "\n";
}

}  // namespace ConsoleStyle
