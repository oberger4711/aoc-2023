#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "utils.h"

std::vector<int> parseNumbers(const std::string& line) {
  std::istringstream is(line);
  std::vector<int> values;
  int v;
  while (!is.eof()) {
    is >> v;
    values.push_back(v);
  }
  return values;
}

int extrapolateNext(const std::vector<std::vector<int>>& derivatives) {
  int value = 0;
  for (int i = derivatives.size() - 2; i >= 0; --i) {
    value = derivatives[i].back() + value;
  }
  return value;
}

int extrapolatePrev(const std::vector<std::vector<int>>& derivatives) {
  int value = 0;
  for (int i = derivatives.size() - 2; i >= 0; --i) {
    value = derivatives[i].front() - value;
  }
  return value;
}

std::vector<std::vector<int>> derive(const std::string& line) {
  std::vector<std::vector<int>> derivatives;
  derivatives.emplace_back(parseNumbers(line));
  bool allZero = true;
  do {
    derivatives.emplace_back();
    auto& values = derivatives[derivatives.size() - 2];
    auto& diffs = derivatives.back();
    diffs.reserve(values.size() - 1);
    allZero = true;
    for (size_t i = 1; i < values.size(); ++i) {
      const int diff = values[i] - values[i - 1];
      allZero &= (diff == 0);
      diffs.push_back(diff);
    }
  } while (!allZero);
  return derivatives;
}

void solveBothParts() {
  std::ifstream ifs("input.txt");
  std::string line;
  long long sumNext = 0, sumPrev = 0;
  while (std::getline(ifs, line)) {
    auto derivatives = derive(line);
    sumNext += extrapolateNext(derivatives);
    sumPrev += extrapolatePrev(derivatives);
  }
  std::cout << "Sum next (part 1): " << sumNext << "\n";
  std::cout << "Sum prev (part 2): " << sumPrev << "\n";
}

void solvePart2() {
  // TODO
}

int main() {
  measureTime(&solveBothParts, "Both parts");
  std::cout << "Done.\n";
  return 0;
}