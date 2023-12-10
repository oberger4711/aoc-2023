#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

const std::string eastConnection = "-LF";
const std::string westConnection = "-J7";
const std::string southConnection = "|7F";
const std::string northConnection = "|LJ";

using Map = std::vector<std::string>;
using Coords = std::array<int, 2>;

void parseMap(std::ifstream &ifs, Map &map) {
  std::string line;
  while (std::getline(ifs, line)) {
    map.push_back(line);
  }
}

Coords findStartCoords(const Map &map) {
  Coords startCoords; // row, col
  bool found = false;
  for (size_t row = 0; row < map.size() && !found; ++row) {
    const auto &line = map[row];
    for (size_t col = 0; col < line.size() && !found; ++col) {
      if (line[col] == 'S') {
        startCoords = {static_cast<int>(row), static_cast<int>(col)};
        found = true;
      }
    }
  }
  assert(found);
  return startCoords;
}

bool isA(const std::string &connections, char ch) {
  return connections.find(ch) != std::string::npos;
}

std::vector<Coords> findStartConnections(const Map &map,
                                         const Coords &startCoords) {
  std::vector<Coords> connections;
  // Left
  {
    char left = map[startCoords[0]][startCoords[1] - 1];
    if (isA(eastConnection, left)) {
      connections.push_back({startCoords[0], startCoords[1] - 1});
    }
  }
  // Right
  {
    char right = map[startCoords[0]][startCoords[1] + 1];
    if (isA(westConnection, right)) {
      connections.push_back({startCoords[0], startCoords[1] + 1});
    }
  }
  // Up
  {
    char up = map[startCoords[0] - 1][startCoords[1]];
    if (isA(southConnection, up)) {
      connections.push_back({startCoords[0] - 1, startCoords[1]});
    }
  }
  // Down
  {
    char down = map[startCoords[0] + 1][startCoords[1]];
    if (isA(northConnection, down)) {
      connections.push_back({startCoords[0] + 1, startCoords[1]});
    }
  }
  assert(connections.size() == 2);
  return connections;
}

Coords getNext(const Map &map, const Coords &cur, const Coords &next) {
  const Coords diff = {next[0] - cur[0], next[1] - cur[1]};
  const char nextCh = map[next[0]][next[1]];
  if (nextCh == '|') {
    return {next[0] + diff[0], next[1] + diff[1]};
  } else if (nextCh == '-') {
    return {next[0] + diff[0], next[1] + diff[1]};
  } else if (nextCh == 'L') {
    return {next[0] + diff[1], next[1] + diff[0]};
  } else if (nextCh == 'J') {
    return {next[0] - diff[1], next[1] - diff[0]};
  } else if (nextCh == '7') {
    return {next[0] + diff[1], next[1] + diff[0]};
  } else if (nextCh == 'F') {
    return {next[0] - diff[1], next[1] - diff[0]};
  } else if (nextCh == 'S') {
    return {0, 0};
  } else {
    const char curCh = map[cur[0]][cur[1]];
    throw std::runtime_error("Reached unknown tile");
  }
}

void solvePart1() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  Map map;
  parseMap(ifs, map);
  // Find start.
  const auto startCoords = findStartCoords(map); // row, col
  const auto startConnections = findStartConnections(map, startCoords);
  size_t count = 0;
  Coords cur = startCoords;
  Coords next = startConnections[0];
  Coords nextNext;
  do {
    nextNext = getNext(map, cur, next);
    std::swap(cur, next);
    std::swap(next, nextNext);
    ++count;
  } while ((cur[0] != startCoords[0]) || (cur[1] != startCoords[1]));
  std::cout << "Steps: " << count / 2 << "\n";
}

Map makeScaledEmptyMap(const Map &map) {
  Map scaledMap;
  std::string empty;
  empty.resize(2 * map[0].size(), ' ');
  std::string dots = empty;
  for (size_t i = 0; i < dots.size(); i += 2) {
    dots[i] = '.';
  }
  scaledMap.reserve(map.size() * 2);
  for (size_t i = 0; i < map.size(); ++i) {
    scaledMap.push_back(dots);
    scaledMap.push_back(empty);
  }
  return scaledMap;
}

char getConnector(const Coords &cur, const Coords &next) {
  char ret;
  if (cur[0] != next[0]) {
    ret = '|';
  } else {
    ret = '-';
  }
  return ret;
}

inline std::array<Coords, 4> getNeighbors(const Coords &coords) {
  return {Coords{coords[0], coords[1] - 1}, Coords{coords[0], coords[1] + 1},
          Coords{coords[0] - 1, coords[1]}, Coords{coords[0] + 1, coords[1]}};
}

bool processPoint(Map &scaledMap, const Coords &coords, size_t &count,
                  std::queue<Coords> &seeds) {
  bool add = false;
  char &ch = scaledMap[coords[0]][coords[1]];
  if (ch == ' ') {
    ch = '#';
    seeds.push(coords);
  } else if (ch == '.') {
    ++count;
    ch = '#';
    seeds.push(coords);
  }
  return add;
}

size_t regionGrow(Map &scaledMap, const Coords &start) {
  std::queue<Coords> seeds;
  size_t count = 0;
  const Coords scaledStart{2 * start[0], 2 * start[1]};
  processPoint(scaledMap, scaledStart, count, seeds);
  while (!seeds.empty()) {
    // std::cout << seeds.size() << "\n";
    const Coords coords = seeds.front();
    seeds.pop();
    // Expand seed.
    if (coords[0] > 0) {
      processPoint(scaledMap, Coords{coords[0] - 1, coords[1]}, count, seeds);
    }
    if (coords[0] < scaledMap.size() - 1) {
      processPoint(scaledMap, Coords{coords[0] + 1, coords[1]}, count, seeds);
    }
    if (coords[1] > 0) {
      processPoint(scaledMap, Coords{coords[0], coords[1] - 1}, count, seeds);
    }
    if (coords[1] < scaledMap[0].size() - 1) {
      processPoint(scaledMap, Coords{coords[0], coords[1] + 1}, count, seeds);
    }
  }
  return count;
}

size_t countDots(const Map &map) {
  size_t count = 0;
  for (const auto &l : map) {
    for (const char &ch : l) {
      if (ch == '.') {
        ++count;
      }
    }
  }
  return count;
}

void printMap(const Map &map) {
  for (const auto &l : map) {
    std::cout << l << "\n";
  }
}

void solvePart2() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  Map map;
  parseMap(ifs, map);
  // Find start.
  const auto startCoords = findStartCoords(map); // row, col
  const auto startConnections = findStartConnections(map, startCoords);
  Map scaledMap = makeScaledEmptyMap(map);
  Coords cur = startCoords;
  Coords next = startConnections[0];
  Coords nextNext;
  size_t pipeLength = 0;
  do {
    scaledMap[2 * cur[0]][2 * cur[1]] = map[cur[0]][cur[1]];
    scaledMap[cur[0] + next[0]][cur[1] + next[1]] = getConnector(cur, next);
    nextNext = getNext(map, cur, next);
    std::swap(cur, next);
    std::swap(next, nextNext);
    pipeLength += 2;
  } while ((cur[0] != startCoords[0]) || (cur[1] != startCoords[1]));
  // Segment the area outside the loop. This is easier to identify than the inlying area.
  regionGrow(scaledMap, Coords{0, 0});
  //printMap(scaledMap);
  // Count remaining dots.
  size_t count = countDots(scaledMap);
  std::cout << "Area: " << count << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}