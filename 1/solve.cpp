#include "utils.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>

inline bool isNumber(char ch) { return 0x30 <= ch && ch < 0x3A; }

void solvePart1() {
  std::ifstream is("input.txt");
  std::string line;
  int sum = 0;
  while (std::getline(is, line)) {
    const char first = *std::find_if(line.begin(), line.end(), &isNumber);
    const char second = *std::find_if(line.rbegin(), line.rend(), &isNumber);
    sum += 10 * (first - 0x30) + (second - 0x30);
  }
  std::cout << "Sum: " << sum << "\n";
}

inline char readDigit(const std::string &line, size_t i, char& out) {
  const char ch = line[i];
  if (isNumber(ch)) {
    out = ch;
    return true;
  } else {
    if (line.size() - i >= 3) {
      if (line.compare(i, 3, "one") == 0) {
        out = '1';
        return true;
      }
      if (line.compare(i, 3, "two") == 0) {
        out = '2';
        return true;
      }
      if (line.compare(i, 3, "six") == 0) {
        out = '6';
        return true;
      }
    }
    if (line.size() - i >= 4) {
      if (line.compare(i, 4, "four") == 0) {
        out = '4';
        return true;
      }
      if (line.compare(i, 4, "five") == 0) {
        out = '5';
        return true;
      }
      if (line.compare(i, 4, "nine") == 0) {
        out = '9';
        return true;
      }
    }
    if (line.size() - i >= 5) {
      if (line.compare(i, 5, "three") == 0) {
        out = '3';
        return true;
      }
      if (line.compare(i, 5, "eight") == 0) {
        out = '8';
        return true;
      }
      if (line.compare(i, 5, "seven") == 0) {
        out = '7';
        return true;
      }
    }
  }
  return false;
}

char findFirst(const std::string &line) {
  char digit;
  for (size_t i = 0; i < line.size(); ++i) {
    if (readDigit(line, i, digit)) {
      return digit;
    }
  }
  throw std::runtime_error("Error: Found no first digit.\n");
}

char findSecond(const std::string &line) {
  char digit;
  for (int i = static_cast<int>(line.size()) - 1; i >= 0; --i) {
    if (readDigit(line, i, digit)) {
      return digit;
    }
  }
  throw std::runtime_error("Error: Found no second digit.\n");
}

// Faster solution of part 2.
void solvePart2() {
  std::ifstream is("input.txt");
  std::string line;
  int sum = 0;
  while (std::getline(is, line)) {
    const char first = findFirst(line);
    const char second = findSecond(line);
    //std::cout << first << second << "\n";
    sum += 10 * (first - 0x30) + (second - 0x30);
  }
  std::cout << "Sum: " << sum << "\n";
}

inline void replaceString(std::string& line, const std::string& searchedString, const std::string& replacedString)
{
  auto pos = line.find(searchedString);
  if (pos != std::string::npos) {
    line.replace(pos, searchedString.size(), replacedString);
  }
}

// Slower but easier to implement solution of part 2.
void solvePart2Replace() {
  std::ifstream is("input.txt");
  std::string line;
  int sum = 0;
  while (std::getline(is, line)) {
    replaceString(line, "one", "1");
    replaceString(line, "two", "2");
    replaceString(line, "three", "3");
    replaceString(line, "four", "4");
    replaceString(line, "five", "5");
    replaceString(line, "six", "6");
    replaceString(line, "seven", "7");
    replaceString(line, "eight", "8");
    replaceString(line, "nine", "9");
    const char first = *std::find_if(line.begin(), line.end(), &isNumber);
    const char second = *std::find_if(line.rbegin(), line.rend(), &isNumber);
    sum += 10 * (first - 0x30) + (second - 0x30);
  }
  std::cout << "Sum: " << sum << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  //measureTime(&solvePart2Replace, "Part 2 (replacing)");
  std::cout << "Done.\n";
  return 0;
}