#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

void solvePart1() {
  int sum = 0;
  std::ifstream is("input.txt");
  // Faster than set and unordered_set for this case. Probably because it uses
  // no dynamic allocations and also O(1). Times: array took 0.519549 ms
  // unordered set took 1.47268 ms
  // set took 1.6033 ms
  std::array<short, 100> winningNumbers;
  winningNumbers.fill(0);
  std::string line;
  int game = 1;
  while (std::getline(is, line)) {
    int points = 0;
    std::istringstream iss(&line.c_str()[10]);
    for (int i = 0; i < 10; ++i) {
      short int win;
      iss >> win;
      winningNumbers[win] = game;
    }
    char separator;
    iss >> separator;
    assert(separator == '|');
    while (!iss.eof()) {
      short int drawn;
      iss >> drawn;
      if (winningNumbers[drawn] == game) {
        points = std::max(1, points * 2);
      }
    }
    sum += points;
    ++game;
  }
  std::cout << "Sum: " << sum << "\n";
}

void solvePart2() {
  std::ifstream is("input.txt");
  constexpr int trimLeft = 10;
  constexpr int numWins = 10;
  constexpr int numLines = 192;

  // std::ifstream is("input_test.txt");
  // constexpr int trimLeft = 9;
  // constexpr int numWins = 5;
  // constexpr int numLines = 6;

  std::array<short, 100> winningNumbers;
  std::array<size_t, numLines + 1> numCopies;
  winningNumbers.fill(0);
  numCopies.fill(1); // Original
  numCopies[0] = 0;
  int game = 1;
  std::string line;
  while (std::getline(is, line)) {
    int points = 0;
    std::istringstream iss(&line.c_str()[trimLeft]);
    for (int i = 0; i < numWins; ++i) {
      short int win;
      iss >> win;
      winningNumbers[win] = game;
    }
    char separator;
    iss >> separator;
    assert(separator == '|');
    while (!iss.eof()) {
      short int drawn;
      iss >> drawn;
      if (winningNumbers[drawn] == game) {
        ++points;
      }
    }
    // std::cout << game << ": " << numCopies[game] << "x points: " << points <<
    // "\n";
    points = std::min(game + points + 1, static_cast<int>(numCopies.size()));
    for (short i = game + 1; i < points; ++i) {
      numCopies[i] += numCopies[game];
    }
    ++game;
  }
  const size_t sum = std::accumulate(
      numCopies.begin(), numCopies.end(),
      static_cast<size_t>(0)); // Card "0" is not counted in the array.
  std::cout << "Sum: " << sum << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}