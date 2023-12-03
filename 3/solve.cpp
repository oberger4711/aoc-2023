#include "utils.h"
#include <fstream>
#include <iostream>
#include <vector>

inline bool isSymbol(char ch) { return (ch != '.' && !isDigit(ch)); }

inline bool checkHasAdjacentSymbol(const std::vector<std::string> &lines,
                                   int row, int colStart, int colEnd) {
  // Above
  if (row > 0) {
    const auto &lineAbove = lines[row - 1];
    for (int col = std::max(0, colStart - 1);
         col < std::min(static_cast<int>(lineAbove.size()), colEnd + 1);
         ++col) {
      if (isSymbol(lineAbove[col])) {
        return true;
      }
    }
  }
  // Current line
  // Left
  const auto &line = lines[row];
  if (colStart > 0 && isSymbol(line[colStart - 1])) {
    return true;
  }
  // Right
  if (colEnd < line.size() && isSymbol(line[colEnd])) {
    return true;
  }
  // Next line
  if (row + 1 < lines.size()) {
    const auto &lineBelow = lines[row + 1];
    for (int col = std::max(0, colStart - 1);
         col < std::min(static_cast<int>(lineBelow.size()), colEnd + 1);
         ++col) {
      if (isSymbol(lineBelow[col])) {
        return true;
      }
    }
  }
  return false;
}

void solvePart1() {
  std::ifstream is("input.txt");
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(is, line)) {
    lines.emplace_back();
    lines.back().swap(line);
  }
  int sum = 0;
  for (int row = 0; row < static_cast<int>(lines.size()); ++row) {
    const auto &line = lines[row];
    int colNumStart = -1;
    for (int col = 0; col < static_cast<int>(line.size()); ++col) {
      const char ch = line[col];
      if (colNumStart == -1) {
        if (isDigit(ch)) {
          colNumStart = col;
        }
      } else {
        if (!isDigit(ch)) {
          // Complete number.
          if (checkHasAdjacentSymbol(lines, row, colNumStart, col)) {
            auto numStr = line.substr(colNumStart, col - colNumStart);
            sum += std::stoi(numStr);
          }
          colNumStart = -1;
        }
      }
    }
    if (colNumStart != -1) {
      // Number ends with end of line.
      if (checkHasAdjacentSymbol(lines, row, colNumStart, line.size())) {
        auto numStr = line.substr(colNumStart, line.size() - colNumStart);
        // std::cout << "Add " << numStr << " (eol)\n";
        sum += std::stoi(numStr);
      }
      colNumStart = -1;
    }
  }
  std::cout << "Sum: " << sum << "\n";
}

struct StarCell {
  int count = 0;
  int product = 1;

  inline void addNumber(int num) {
    product *= num;
    ++count;
  }

  inline int reset() {
    int ret = 0;
    if (count == 2) {
      ret = product;
    }
    count = 0;
    product = 1;
    return ret;
  }
};

struct StarLinesBuffer {
  std::vector<StarCell> prevStarLine;
  std::vector<StarCell> curStarLine;
  std::vector<StarCell> nextStarLine;

  StarLinesBuffer(size_t cols)
      : prevStarLine(cols), curStarLine(cols), nextStarLine(cols) {}

  inline void updatePrevLine(const std::string &prevLine, int number,
                             int colStart, int colEnd) {
    for (int col = std::max(0, colStart - 1);
         col < std::min(static_cast<int>(prevLine.size()), colEnd + 1); ++col) {
      if (prevLine[col] == '*') {
        prevStarLine[col].addNumber(number);
      }
    }
  }

  inline void updateCurLine(const std::string &curLine, int number,
                            int colStart, int colEnd) {
    // Left
    if (colStart > 0 && curLine[colStart - 1] == '*') {
      curStarLine[colStart - 1].addNumber(number);
    }
    // Right
    if (colEnd < curLine.size() && curLine[colEnd] == '*') {
      curStarLine[colEnd].addNumber(number);
    }
  }

  inline void updateNextLine(const std::string &nextLine, int number,
                             int colStart, int colEnd) {
    for (int col = std::max(0, colStart - 1);
         col < std::min(static_cast<int>(nextLine.size()), colEnd + 1); ++col) {
      if (nextLine[col] == '*') {
        nextStarLine[col].addNumber(number);
      }
    }
  }

  inline void update(const std::vector<std::string> &lines, int row, int number,
                     int colStart, int colEnd) {
    if (row > 0) {
      updatePrevLine(lines[row - 1], number, colStart, colEnd);
    }
    updateCurLine(lines[row], number, colStart, colEnd);
    if (row < lines.size()) {
      updateNextLine(lines[row + 1], number, colStart, colEnd);
    }
  }

  inline int evalPrevLine() {
    int sum = 0;
    for (auto &cell : prevStarLine) {
      sum += cell.reset();
    }
    return sum;
  }

  inline void cycle() {
    std::swap(prevStarLine, curStarLine);
    std::swap(curStarLine, nextStarLine);
  }
};

void solvePart2() {
  std::ifstream is("input.txt");
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(is, line)) {
    lines.emplace_back();
    lines.back().swap(line);
  }
  StarLinesBuffer buffer(lines[0].size());
  int sum = 0;
  for (int row = 0; row < static_cast<int>(lines.size()); ++row) {
    const auto &line = lines[row];
    int colNumStart = -1;
    for (int col = 0; col < static_cast<int>(line.size()); ++col) {
      const char ch = line[col];
      if (colNumStart == -1) {
        if (isDigit(ch)) {
          colNumStart = col;
        }
      } else {
        if (!isDigit(ch)) {
          // Complete number.
          const auto number =
              std::stoi(line.substr(colNumStart, col - colNumStart));
          buffer.update(lines, row, number, colNumStart, col);
          colNumStart = -1;
        }
      }
    }
    if (colNumStart != -1) {
      // Number ends with end of line.
      const auto number =
          std::stoi(line.substr(colNumStart, line.size() - colNumStart));
      buffer.update(lines, row, number, colNumStart, line.size());
      colNumStart = -1;
    }
    sum += buffer.evalPrevLine();
    buffer.cycle();
  }
  std::cout << "Sum: " << sum << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}