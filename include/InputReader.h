#ifndef INPUT_READER_H
#define INPUT_READER_H

#include <string>

// Safe console input helpers for the presentation layer (main.cpp).
namespace InputReader {

void clearInputBuffer();
bool readInt(const std::string& prompt, int& out);
bool readIntInRange(const std::string& prompt, int minValue, int maxValue, int& out);
bool readLine(const std::string& prompt, std::string& out);
bool readRequiredLine(const std::string& prompt, std::string& out, const std::string& fieldName);
bool readYesNo(const std::string& prompt, bool& out);
void pauseBeforeReturning();

}  // namespace InputReader

#endif
