#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

constexpr short int ZZZ = 25 * 26 * 26 + 25 * 26 + 25;
constexpr short int MAP_SIZE = ZZZ + 1;

using Node = std::array<short int, 2>; // Destination index for L = 0 and R = 1.
using NodeMap = std::array<Node, MAP_SIZE>;

short int tripletToInt(const char *triplet) {
  int ret = triplet[0] - 'A';
  ret = ret * 26 + triplet[1] - 'A';
  ret = ret * 26 + triplet[2] - 'A';
  return ret;
}

inline void loadInput1(std::ifstream &ifs, std::string &input,
                       NodeMap &nodeMap) {
  std::getline(ifs, input);
  std::string line;
  std::getline(ifs, line); // Empty
  // Load graph.
  std::string src, destL, destR;
  char equals;
  while (std::getline(ifs, line)) {
    std::istringstream iss(line);
    iss >> src >> equals >> destL >> destR;
    // std::cout << src << ": " << destL << ", " << destR << "\n";
    auto &node = nodeMap[tripletToInt(&src[0])];
    node[0] = tripletToInt(&destL[1]); // Cut off leading "(".
    node[1] = tripletToInt(&destR[0]);
  }
}

void solvePart1() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  std::string input;
  NodeMap nodeMap;
  loadInput1(ifs, input, nodeMap);

  // Traverse graph using input.
  bool found = false;
  short int cur = tripletToInt("AAA");
  int steps = 0;
  do {
    for (size_t i = 0; i < input.size() && !found; ++i) {
      short int dir = (input[i] == 'L' ? 0 : 1);
      cur = nodeMap[cur][dir];
      found = (cur == ZZZ);
      ++steps;
    }
  } while (!found);
  std::cout << "Steps: " << steps << "\n";
}

inline void loadInput2(std::ifstream &ifs, std::string &input,
                       std::vector<short int> &startNodes, NodeMap &nodeMap) {
  std::getline(ifs, input);
  std::string line;
  std::getline(ifs, line); // Empty
  // Load graph.
  std::string src, destL, destR;
  char equals;
  while (std::getline(ifs, line)) {
    std::istringstream iss(line);
    iss >> src >> equals >> destL >> destR;
    // std::cout << src << ": " << destL << ", " << destR << "\n";
    short int srcInt = tripletToInt(&src[0]);
    auto &node = nodeMap[srcInt];
    node[0] = tripletToInt(&destL[1]); // Cut off leading "(".
    node[1] = tripletToInt(&destR[0]);
    if (src[2] == 'A') {
      // Nodes ending on A are starting nodes.
      startNodes.push_back(srcInt);
    }
  }
}

inline bool isFinalNode(short int nodeInt) { return (nodeInt % 26) == 25; }

constexpr inline unsigned int
encodeState(short int nodeInt, unsigned short int inputStep, size_t size) {
  return nodeInt * size + inputStep;
}

struct FinalStepFunc {
  size_t m;
  size_t c;
  size_t n = 0;
  FinalStepFunc(int m_, int c_) : m(m_), c(c_) {}
  size_t get() { return m * n + c; }
  size_t next() {
    ++n;
    return get();
  }
  size_t update(size_t value) {
    if (value > c) {
      n = (value - c) / m;
    } else {
      n = 0;
    }
    return get();
  }
};

std::vector<FinalStepFunc> deriveFuncs(const NodeMap &nodeMap,
                                       const std::string &input,
                                       std::vector<short int> &curs) {
  std::vector<FinalStepFunc> funcs;
  // Find out when the pattern repeats: The first step at which the state of
  // (node ID, input index) has been visited already.
  std::vector<bool> map; // true means that we were in this state already.
  map.resize(encodeState(ZZZ, nodeMap.size(), nodeMap.size()));
  for (auto &cur : curs) {
    int found = 0;
    size_t steps = 1;
    std::fill(map.begin(), map.end(), false);
    map[encodeState(cur, 0, nodeMap.size())] = true;
    std::vector<size_t> finalSteps;
    do {
      for (size_t i = 0; i < input.size() && found < 3; ++i, ++steps) {
        short int dir = (input[i] == 'L' ? 0 : 1);
        cur = nodeMap[cur][dir];
        if (isFinalNode(cur)) {
          finalSteps.push_back(steps);
        }
        if (map[encodeState(cur, i, nodeMap.size())]) {
          ++found;
          std::fill(map.begin(), map.end(), false);
        }
        map[encodeState(cur, i, nodeMap.size())] = true;
      }
    } while (found < 3);
    const int m = finalSteps[1] - finalSteps[0];
    const int c = finalSteps[0];
    funcs.emplace_back(m, c);
    if (m * 2 + c != finalSteps[2]) {
      throw std::runtime_error("Could not derive linear function parameters.");
    }
  }
  return funcs;
}

size_t findCommonFinalStep(std::vector<FinalStepFunc> &funcs) {
  bool found = false;
  for (size_t mainStep = funcs[0].get(); true; mainStep = funcs[0].next()) {
    found = true;
    for (size_t i = 1; i < funcs.size() && found; ++i) {
      auto &func = funcs[i];
      found &= (func.update(mainStep) == mainStep);
    }
    if (found) {
      return mainStep;
    }
  }
}

void solvePart2() {
  std::ifstream ifs("input.txt");
  //std::ifstream ifs("input_test2.txt");
  std::string input;
  NodeMap nodeMap;
  std::vector<short int> curs;
  loadInput2(ifs, input, curs, nodeMap);
  auto funcs = deriveFuncs(nodeMap, input, curs);
  const auto steps = findCommonFinalStep(funcs);
  std::cout << "Steps: " << steps << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}