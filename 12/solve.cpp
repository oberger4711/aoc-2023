#include "utils.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Processor {
  size_t processLine(const std::string &line, unsigned short startLine,
                     unsigned char brokenCount, const std::vector<int> &broken,
                     unsigned char startBroken) {
    size_t newSolutions = 0;
    // Use memoization to avoid redundant computations.
    const auto query = makeQuery(startLine, brokenCount, startBroken);
    auto queryMatch = memory.find(query);
    if (queryMatch != memory.end()) {
      return queryMatch->second;
    }
    // The branch has not yet been computed -> explore it.
    for (size_t i = startLine; i < line.size(); ++i) {
      const char ch = line[i];
      if (ch == '.') {
        if (brokenCount > 0) {
          if (startBroken < broken.size()) {
            auto nextBroken = broken[startBroken];
            if (brokenCount == nextBroken) {
              ++startBroken;
              brokenCount = 0;
            } else {
              // Detected inconsistency.
              memory[query] = newSolutions;
              return newSolutions;
            }
          } else {
            // Detected inconsistency.
            memory[query] = newSolutions;
            return newSolutions;
          }
        }
      } else if (ch == '#') {
        if (startBroken == broken.size()) {
          // Detected inconsistency.
          memory[query] = newSolutions;
          return newSolutions;
        }
        ++brokenCount;
        if (brokenCount > broken[startBroken]) {
          // Detected inconsistency.
          memory[query] = newSolutions;
          return newSolutions;
        }
      } else if (ch == '?') {
        {
          // Act as if it is a '#'.
          if (startBroken < broken.size() &&
              brokenCount + 1 <= broken[startBroken]) {
            newSolutions +=
                processLine(line, i + 1, brokenCount + 1, broken, startBroken);
          }
        }
        {
          // Act as if it is a '.'.
          if (startBroken < broken.size()) {
            auto nextBroken = broken[startBroken];
            if (brokenCount == nextBroken) {
              ++startBroken;
              brokenCount = 0;
            } else if (brokenCount != 0) {
              // Detected inconsistency.
              memory[query] = newSolutions;
              return newSolutions;
            }
          }
          newSolutions += processLine(line, i + 1, 0, broken, startBroken);
          memory[query] = newSolutions;
          return newSolutions;
        }
        memory[query] = newSolutions;
        return newSolutions;
      }
    }
    // Reached end of the line.
    if (startBroken == broken.size() || // All blocks finished already.
        (brokenCount == broken[startBroken] &&
         startBroken ==
             broken.size() - 1)) { // Finishing last block on line ending.
      ++newSolutions;
    }
    memory[query] = newSolutions;
    return newSolutions;
  }

private:
  using Query = unsigned long long;
  std::unordered_map<Query, size_t> memory;

  Query makeQuery(unsigned short startLine, unsigned short brokenCount,
                  unsigned short startBroken) {
    Query ret = 0;
    static_assert(sizeof(Query) >= 3 * sizeof(unsigned short),
                  "Query type must be large enough to fit parameters into.");
    ret |= startLine;
    ret = ret << 8 * sizeof(decltype(brokenCount));
    ret |= brokenCount;
    ret = ret << 8 * sizeof(decltype(startBroken));
    ret |= startBroken;
    return ret;
  }
};

size_t processLine(const std::string &line, const std::vector<int> &broken) {
  return Processor().processLine(line, 0, 0, broken, 0);
}

std::vector<int> parseBroken(std::string &brokenStr) {
  std::replace(brokenStr.begin(), brokenStr.end(), ',', ' ');
  std::istringstream iss(brokenStr);
  std::vector<int> ret;
  while (!iss.eof()) {
    int num;
    iss >> num;
    ret.push_back(num);
  }
  return ret;
}

void solvePart1() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  std::string line;
  size_t sum = 0;
  while (std::getline(ifs, line)) {
    std::istringstream iss(line);
    std::string map, brokenStr;
    iss >> map >> brokenStr;
    auto broken = parseBroken(brokenStr);
    auto count = processLine(map, broken);
    // std::cout << line << " --> " << count << "\n";
    sum += count;
  }
  std::cout << "Sum: " << sum << "\n";
}

void solvePart2() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  std::string line;
  size_t sum = 0;
  while (std::getline(ifs, line)) {
    std::istringstream iss(line);
    std::string map, brokenStr;
    iss >> map >> brokenStr;
    std::string mapUnfolded = map, brokenStrUnfolded = brokenStr;
    for (int i = 1; i < 5; ++i) {
      mapUnfolded = mapUnfolded + "?" + map;
      brokenStrUnfolded = brokenStrUnfolded + "," + brokenStr;
    }
    auto broken = parseBroken(brokenStrUnfolded);
    auto count = processLine(mapUnfolded, broken);
    //std::cout << line << " --> " << count << "\n";
    sum += count;
  }
  std::cout << "Sum: " << sum << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}