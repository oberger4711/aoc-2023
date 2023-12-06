#include "utils.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using num = long long;

inline std::vector<int> parseList(const std::string &line, size_t start) {
  int num;
  std::vector<int> list;
  std::istringstream iss(&line.c_str()[start]);
  while (!iss.eof()) {
    iss >> num;
    list.push_back(num);
  }
  return list;
}

inline std::pair<num, num> solveQuadratic(num duration, num minDist) {
  const double d = static_cast<double>(duration);
  const double w = static_cast<double>(minDist);
  double x1 = (-d - std::sqrt(d * d - 4 * w)) / -2;
  double x2 = (-d + std::sqrt(d * d - 4 * w)) / -2;
  if (x2 < x1) {
    std::swap(x1, x2);
  }
  //std::cout << x1 << ", " << x2 << "\n";
  num i1 = static_cast<num>(x1) - 1;
  num i2 = static_cast<num>(x2) + 1;
  // Deal with floating point boundary values.
  while (i1 * (duration - i1) <= minDist) {
    ++i1;
  }
  while (i2 * (duration - i2) <= minDist) {
    --i2;
  }
  return std::make_pair(i1, i2);
}

std::ostream &operator<<(std::ostream &os,
                         const std::pair<double, double> &interval) {
  os << "[" << interval.first << ", " << interval.second << "]";
  return os;
}

void solvePart1() {
  //std::ifstream is("input_test.txt");
  std::ifstream is("input.txt");
  constexpr int lineStart = 10;
  std::string line;
  std::getline(is, line);
  auto durations = parseList(line, lineStart);
  std::getline(is, line);
  auto distances = parseList(line, lineStart);
  size_t prod = 1;
  for (size_t i = 0; i < durations.size(); ++i) {
    const int duration = durations[i];
    const int distance = distances[i];
    const auto solutions = solveQuadratic(duration, distance);
    const auto numSolutions = solutions.second + 1 - solutions.first;
    prod *= static_cast<size_t>(numSolutions);
    //std::cout << duration << ", " << distance << ": " << solutions << " (" << numSolutions << "x)\n";
  }
  std::cout << "Product: " << prod << "\n";
}

void solvePart2() {
  //const auto sol = solveQuadratic(71530, 940200); // Test
  const auto sol = solveQuadratic(56717999, 334113513502430);
  const auto num = sol.second  + 1 - sol.first;
  std::cout << "Number: " << num << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}