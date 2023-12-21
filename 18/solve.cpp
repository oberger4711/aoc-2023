#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

// Need to use a larger integer type for part 2.
using Coords = Coords_<long long>;

const Coords LEFT(Coords::Left());
const Coords DOWN(Coords::Down());
const Coords RIGHT(Coords::Right());
const Coords UP(Coords::Up());

// Lookup table to handle direction of part 2 as in part 1.
std::array<char, 4> hexToCh = {'R', 'D', 'L', 'U'};

const Coords &chToDir(char ch) {
  if (ch == 'L') {
    return LEFT;
  } else if (ch == 'D') {
    return DOWN;
  } else if (ch == 'R') {
    return RIGHT;
  } else if (ch == 'U') {
    return UP;
  } else {
    throw std::runtime_error("Unknown direction.");
  }
}

// Compute the area of a simple polygon (i.e. non-self-intersecting).
long long shoeLace(const std::vector<Coords> &corners) {
  // https://en.wikipedia.org/wiki/Shoelace_formula
  long long sum = 0;
  // The formula assumes c_0 == c_n but we don't have such a c_0 in the vector.
  // Therefore add it explicitly to the sum.
  const Coords cN = corners.back();
  const Coords c0 = corners[0];
  sum += cN.col * c0.row - c0.col * cN.row;
  for (size_t i = 0; i < corners.size() - 1; ++i) {
    const auto &cCur = corners[i];
    const auto &cNext = corners[i + 1];
    sum += cCur.col * cNext.row - cNext.col * cCur.row;
  }
  assert(sum % 2 == 0); // Area must be integer or there is a bug somewhere.
  return sum / 2;
}

// Compute the area of a simple polygon (i.e. non-self-intersecting).
long long shoeLaceIncrement(const Coords &cur, const Coords &next) {
  // https://en.wikipedia.org/wiki/Shoelace_formula
  return cur.col * next.row - next.col * cur.row;
}

long long shoeLaceArea(long long sum) {
  assert(sum > 0);
  assert(sum % 2 == 0); // Area must be integer or there is a bug somewhere.
  return sum / 2;
}

class Processor {
public:
  void processStep(char dir, int steps) {
    addDirHistory(dir);
    if (firstDir != ' ') {
      // Not in the first iteration.
      const Coords curCcwise = makeCornerCoordsCcwise();
      const Coords curCwise = makeCornerCoordsCwise();
      if (firstCorner) {
        sumCcwise += shoeLaceIncrement(prevCorner->ccwise, curCcwise);
        sumCwise += shoeLaceIncrement(prevCorner->cwise, curCwise);
        // std::cout << sumCcwise << ", " << sumCwise << "\n";
      } else {
        firstCorner = CoordsPair{curCcwise, curCwise};
      }
      prevCorner = CoordsPair{curCcwise, curCwise};
    }
    pos += chToDir(dir) * steps;
    if (firstDir == ' ') {
      firstDir = dir;
    }
  }

  Coords makeCornerCoordsCcwise() {
    Coords ret(pos);
    if (dirLR == 'L') {
      if (dirUD == 'U') {
        ret.plusEquals(0, 1);
      } else if (dirUD != 'D') {
        throw std::runtime_error("Incomplete history. dirUD empty.");
      }
    } else if (dirLR == 'R') {
      if (dirUD == 'D') {
        ret.plusEquals(1, 0);
      } else if (dirUD == 'U') {
        ret.plusEquals(1, 1);
      } else {
        throw std::runtime_error("Incomplete history. dirUD empty.");
      }
    } else {
      throw std::runtime_error("Incomplete history. dirLR empty.");
    }
    return ret;
  }

  Coords makeCornerCoordsCwise() {
    Coords ret(pos);
    // Cwise
    if (dirLR == 'L') {
      if (dirUD == 'U') {
        ret.plusEquals(1, 0);
      } else if (dirUD == 'D') {
        ret.plusEquals(1, 1);
      } else {
        throw std::runtime_error("Incomplete history. dirUD empty.");
      }
    } else if (dirLR == 'R') {
      if (dirUD == 'D') {
        ret.plusEquals(0, 1);
      } else if (dirUD != 'U') {
        throw std::runtime_error("Incomplete history. dirUD empty.");
      }
    } else {
      throw std::runtime_error("Incomplete history. dirLR empty.");
    }
    return ret;
  }

  long long finalize() {
    assert(pos == Coords(0, 0)); // Sanity check.
    addDirHistory(firstDir);
    const Coords curCcwise = makeCornerCoordsCcwise();
    const Coords curCwise = makeCornerCoordsCwise();
    sumCcwise += shoeLaceIncrement(prevCorner->ccwise, curCcwise);
    sumCwise += shoeLaceIncrement(prevCorner->cwise, curCwise);
    // Now also connect last and first corner.
    sumCcwise += shoeLaceIncrement(curCcwise, firstCorner->ccwise);
    sumCwise += shoeLaceIncrement(curCwise, firstCorner->cwise);
    return std::max(shoeLaceArea(sumCcwise), shoeLaceArea(sumCwise));
  }

private:
  struct CoordsPair {
    Coords ccwise;
    Coords cwise;
  };
  Coords pos{0, 0};
  char dirUD{' '};
  char dirLR{' '};
  std::optional<CoordsPair> firstCorner;
  std::optional<CoordsPair> prevCorner;
  long long sumCcwise = 0; // Area lies to the left w.r.t. direction.
  long long sumCwise = 0;  // Area lies to the right w.r.t. direction.
  char firstDir{' '};

  inline void addDirHistory(char dir) {
    if (dir == 'L' || dir == 'R') {
      dirLR = dir;
    } else {
      dirUD = dir;
    }
  }
};

void solvePart1() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  std::string line;
  Processor processor;
  while (std::getline(ifs, line)) {
    std::istringstream iss(line);
    char dir;
    int steps;
    iss >> dir >> steps;
    processor.processStep(dir, steps);
  }
  const auto area = processor.finalize();
  std::cout << "Area: " << area << "\n";
  assert(area == 40131);
}

void solvePart2() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  std::string line;
  Processor processor;
  while (std::getline(ifs, line)) {
    std::istringstream iss(line);
    char chJunk;
    int iJunk;
    std::string str;
    iss >> chJunk >> iJunk >> str;
    // Extract dir and steps from hex string.
    const char dir = hexToCh[str[7] - '0'];
    const std::string stepsHex = str.substr(2, 5); // Cut off () and #.
    const int steps = std::stoi(stepsHex, nullptr, 16);
    // std::cout << dir << " " << steps << "\n";
    processor.processStep(dir, steps);
  }
  const auto area = processor.finalize();
  std::cout << "Area: " << area << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}