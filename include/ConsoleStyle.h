#ifndef CONSOLE_STYLE_H
#define CONSOLE_STYLE_H

#include <iostream>
#include <string>

// ANSI colors for a readable terminal UI (Windows VT mode enabled in main).
namespace ConsoleStyle {

void enableColors();
void reset();

void printBannerLine(char fill = '=');
void printTitle(const std::string& text);
void printAuthor(const std::string& text);
void printSubtitle(const std::string& text);
void printSection(const std::string& text);
void printMenuItem(int option, const std::string& text);
void printSuccess(const std::string& text);
void printError(const std::string& text);
void printWarning(const std::string& text);
void printInfo(const std::string& text);

}  // namespace ConsoleStyle

#endif
