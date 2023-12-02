#include "utils.h"
#include <fstream>
#include <iostream>
#include <sstream>

inline int colorToInt(char c) {
  if (c == 'r') {
    return 0;
  } else if (c == 'g') {
    return 1;
  } else if (c == 'b') {
    return 2;
  } else {
    throw std::runtime_error(std::string("Invalid color ") + c);
  }
}

void solvePart1() {
  std::ifstream is("input.txt");
  std::string line;
  std::array<int, 3> maxNums = {12, 13, 14};
  int sum = 0;
  int id = 0;
  while (std::getline(is, line)) {
    ++id;
    bool possible = true;
    auto start = line.find(':') + 1; // Skip game id.
    int num;
    std::string color;
    std::istringstream is(&line.c_str()[start]);
    do {
      is >> num >> color;
      if (num > maxNums[colorToInt(color[0])]) {
        possible = false;
        break;
      }
    } while (!is.eof());
    if (possible) {
      sum += id;
    }
  }
  std::cout << sum << "\n";
}

void solvePart2() {
  std::ifstream is("input.txt");
  std::string line;
  int sum = 0;
  int id = 0;
  while (std::getline(is, line)) {
    ++id;
    std::array<int, 3> counts = {0, 0, 0};
    auto start = line.find(':') + 1; // Skip game id.
    int num;
    std::string color;
    std::istringstream is(&line.c_str()[start]);
    do {
      is >> num >> color;
      int &count = counts[colorToInt(color[0])];
      count = std::max(count, num);
    } while (!is.eof());
    sum += counts[0] * counts[1] * counts[2];
  }
  std::cout << sum << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}