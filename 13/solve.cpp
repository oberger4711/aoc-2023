#include "utils.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using Pattern = std::vector<std::string>;
using Patterns = std::vector<Pattern>;

std::ostream &operator<<(std::ostream &os, const Pattern &pattern) {
  for (const auto &line : pattern) {
    os << line << "\n";
  }
  return os;
}

Patterns parsePatterns(std::ifstream &ifs) {
  Patterns patterns;
  std::string line;
  patterns.emplace_back();
  Pattern *curPattern = &patterns.back();
  while (std::getline(ifs, line)) {
    if (line.empty()) {
      patterns.emplace_back();
      curPattern = &patterns.back();
    } else {
      curPattern->push_back(line);
    }
  }
  return patterns;
}

int findVerticalReflection(const Pattern &pattern, int smudges = 0) {
  const size_t rows = pattern.size();
  const size_t cols = pattern[0].size();
  // Check for reflections between column colLeft and colRight.
  for (size_t colLeft = 0; colLeft < cols - 1; ++colLeft) {
    const size_t colRight = colLeft + 1;
    int smudgesLeft = smudges;
    bool consistent = true;
    for (size_t row = 0; row < rows && consistent; ++row) {
      const auto &line = pattern[row];
      const size_t maxOffset = std::min(cols - colRight - 1, colLeft);
      for (size_t colOffset = 0; colOffset <= maxOffset && consistent;
           ++colOffset) {
        if (line[colLeft - colOffset] != line[colRight + colOffset]) {
          --smudgesLeft;
          if (smudgesLeft < 0) {
            consistent = false;
          }
        }
      }
    }
    if (consistent && smudgesLeft == 0) {
      return static_cast<int>(colLeft);
    }
  }
  // No consistent match.
  return -1;
}

int findHorizontalReflection(const Pattern &pattern, int smudges = 0) {
  const size_t rows = pattern.size();
  const size_t cols = pattern[0].size();
  // Check for reflections between rows rowTop and rowBot.
  for (size_t rowTop = 0; rowTop < rows - 1; ++rowTop) {
    const size_t rowBot = rowTop + 1;
    int smudgesLeft = smudges;
    bool consistent = true;
    for (size_t col = 0; col < cols; ++col) {
      const size_t maxOffset = std::min(rows - rowBot - 1, rowTop);
      for (size_t rowOffset = 0; rowOffset <= maxOffset && consistent;
           ++rowOffset) {
        const auto &lineTop = pattern[rowTop - rowOffset];
        const auto &lineBot = pattern[rowBot + rowOffset];
        if (lineTop[col] != lineBot[col]) {
          --smudgesLeft;
          if (smudgesLeft < 0) {
            consistent = false;
          }
        }
      }
    }
    if (consistent && smudgesLeft == 0) {
      return static_cast<int>(rowTop);
    }
  }
  // No consistent match.
  return -1;
}

void solve(const Patterns &patterns, int smudges) {
  size_t sum = 0;
  for (const auto &pattern : patterns) {
    // std::cout << pattern;
    const auto verticalReflection = findVerticalReflection(pattern, smudges);
    if (verticalReflection >= 0) {
      // std::cout << "Vertical: " << verticalReflection << "\n\n";
      sum += verticalReflection + 1;
    } else {
      const auto horizontalReflection =
          findHorizontalReflection(pattern, smudges);
      // std::cout << "Horizontal: " << horizontalReflection << "\n\n";
      sum += 100 * (horizontalReflection + 1);
      // Sanity check
      if (horizontalReflection < 0) {
        throw std::runtime_error(
            "Found no reflection at all for this pattern!");
      }
    }
  }
  std::cout << smudges << " smudges: "
            << " sum: " << sum << "\n";
}

void solveBothParts() {
  std::ifstream ifs("input.txt");
  const auto patterns = parsePatterns(ifs);
  solve(patterns, 0); // Part 1
  solve(patterns, 1); // Part 2
}

int main() {
  measureTime(&solveBothParts, "Both parts");
  std::cout << "Done.\n";
  return 0;
}
