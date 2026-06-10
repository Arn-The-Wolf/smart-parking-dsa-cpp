#include "InputReader.h"

#include <iostream>
#include <limits>

namespace InputReader {

void clearInputBuffer() {
    std::cin.clear();
    if (std::cin.eof()) {
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool readInt(const std::string& prompt, int& out) {
    while (true) {
        std::cout << prompt;
        if (std::cin >> out) {
            clearInputBuffer();
            return true;
        }
        if (std::cin.eof()) {
            std::cout << "\nEnd of input detected.\n";
            return false;
        }
        std::cout << "Invalid input. Please enter a whole number (no letters or symbols).\n";
        clearInputBuffer();
    }
}

bool readIntInRange(const std::string& prompt, int minValue, int maxValue, int& out) {
    while (true) {
        if (!readInt(prompt, out)) {
            return false;
        }
        if (out >= minValue && out <= maxValue) {
            return true;
        }
        std::cout << "Please enter a number between " << minValue << " and " << maxValue << ".\n";
    }
}

bool readLine(const std::string& prompt, std::string& out) {
    std::cout << prompt;
    if (!std::getline(std::cin, out)) {
        if (std::cin.eof()) {
            std::cout << "\nEnd of input detected.\n";
        }
        return false;
    }

    const auto start = out.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        out.clear();
    } else {
        const auto end = out.find_last_not_of(" \t\r\n");
        out = out.substr(start, end - start + 1);
    }
    return true;
}

bool readRequiredLine(const std::string& prompt, std::string& out, const std::string& fieldName) {
    while (true) {
        if (!readLine(prompt, out)) {
            return false;
        }
        if (!out.empty()) {
            return true;
        }
        std::cout << fieldName << " cannot be empty. Please try again.\n";
    }
}

bool readYesNo(const std::string& prompt, bool& out) {
    while (true) {
        std::string answer;
        if (!readLine(prompt, answer)) {
            return false;
        }
        if (answer == "y" || answer == "Y" || answer == "yes" || answer == "Yes") {
            out = true;
            return true;
        }
        if (answer == "n" || answer == "N" || answer == "no" || answer == "No") {
            out = false;
            return true;
        }
        std::cout << "Please enter y (yes) or n (no).\n";
    }
}

void pauseBeforeReturning() {
    std::cout << "\nPress Enter to return to the menu...";
    std::string ignored;
    std::getline(std::cin, ignored);
}

}  // namespace InputReader
