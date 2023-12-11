#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using num = long long;
using Coords = std::array<num, 2>; // Row, col

std::vector<num> buildRowMap(const std::vector<std::string> &lines,
                             num expansion) {
  std::vector<num> rowMap;
  rowMap.reserve(lines.size());
  num offset = 0;
  for (size_t row = 0; row < lines.size(); ++row) {
    const auto &line = lines[row];
    if (line.find('#') == std::string::npos) {
      // Empty
      offset += expansion - 1;
    }
    rowMap[row] = offset + row;
  }
  return rowMap;
}

std::vector<num> buildColMap(const std::vector<std::string> &lines,
                             num expansion) {
  std::vector<num> colMap;
  colMap.reserve(lines[0].size());
  size_t offset = 0;
  for (size_t col = 0; col < lines[0].size(); ++col) {
    bool foundGalaxy = false;
    for (size_t row = 0; row < lines.size(); ++row) {
      const auto &line = lines[row];
      if (line[col] == '#') {
        foundGalaxy = true;
      }
    }
    if (!foundGalaxy) {
      // Empty
      offset += expansion - 1;
    }
    colMap[col] = offset + col;
  }
  return colMap;
}

std::vector<Coords> extractGalaxyCoords(const std::vector<std::string> &lines) {
  std::vector<Coords> coords;
  for (size_t row = 0; row < lines.size(); ++row) {
    const auto &line = lines[row];
    for (size_t col = 0; col < line.size(); ++col) {
      if (line[col] == '#') {
        coords.push_back({static_cast<num>(row), static_cast<num>(col)});
      }
    }
  }
  return coords;
}

void expandGalaxyCoords(std::vector<Coords> &coords,
                        const std::vector<num> &rowMap,
                        const std::vector<num> &colMap) {
  for (auto &c : coords) {
    c[0] = rowMap[c[0]];
    c[1] = colMap[c[1]];
  }
}

size_t computePairwiseDistances(const std::vector<Coords> &coords) {
  size_t sum = 0;
  for (size_t i = 0; i < coords.size() - 1; ++i) {
    for (size_t j = i + 1; j < coords.size(); ++j) {
      const auto &src = coords[i];
      const auto &dest = coords[j];
      // Manhattan distance
      const auto dist = std::abs(dest[0] - src[0]) + std::abs(dest[1] - src[1]);
      sum += dist;
    }
  }
  return sum;
}

void solveBothParts() {
  std::ifstream ifs("input.txt");
  //std::ifstream ifs("input_test.txt");
  std::string line;
  std::vector<std::string> lines;
  while (std::getline(ifs, line)) {
    lines.push_back(line);
  }
  const std::vector<num> expansions{2, 1'000'000};
  auto ogCoords = extractGalaxyCoords(lines);
  for (num i : expansions) {
    auto coords = ogCoords;
    // These lookup tables map text coordinates -> expanded coordinates.
    auto rowMap = buildRowMap(lines, i);
    auto colMap = buildColMap(lines, i);
    expandGalaxyCoords(coords, rowMap, colMap);
    const auto sum = computePairwiseDistances(coords);
    std::cout << "Expansion " << i << ": " << sum << "\n";
  }
}

int main() {
  measureTime(&solveBothParts, "Both parts");
  std::cout << "Done.\n";
  return 0;
}