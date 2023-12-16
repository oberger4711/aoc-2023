#include "utils.h"
#include <fstream>
#include <iostream>
#include <queue>
#include <string>

struct Beam {
  // Bits encoding direction fitting into one byte.
  enum class DirectionBF { UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8 };

  Coords coords; // Row, col
  Coords dir;    // Row, col

  Beam(const Coords &coords_, const Coords &dir_)
      : coords(coords_), dir(dir_) {}

  DirectionBF dirBF() const {
    if (dir.row > 0) {
      return DirectionBF::UP;
    } else if (dir.row < 0) {
      return DirectionBF::DOWN;
    } else if (dir.col > 0) {
      return DirectionBF::RIGHT;
    } else if (dir.col < 0) {
      return DirectionBF::LEFT;
    } else {
      throw std::runtime_error("Unknown direction.");
    }
  }
};

std::vector<std::vector<char>>
makeBeamDirectionsBFLike(const std::vector<std::string> &lines) {
  std::vector<std::vector<char>> beamDirections;
  std::vector<char> darkLine;
  darkLine.resize(lines[0].size(), 0);
  for (size_t i = 0; i < lines.size(); ++i) {
    beamDirections.push_back(darkLine);
  }
  return beamDirections;
}

void printBeamDirections(std::vector<std::vector<char>> &beamDirections) {
  for (auto &line : beamDirections) {
    for (auto ch : line) {
      if (ch != 0) {
        std::cout << "#";
      } else {
        std::cout << " ";
      }
    }
    std::cout << "\n";
  }
}

size_t countLit(std::vector<std::vector<char>> &beamDirections) {
  size_t lit = 0;
  for (auto &line : beamDirections) {
    for (auto ch : line) {
      if (ch != 0) {
        ++lit;
      }
    }
  }
  return lit;
}

size_t simulateBeam(const std::vector<std::string> &lines,
                    const Beam &initalBeam) {
  auto beamDirections = makeBeamDirectionsBFLike(lines);
  const size_t rows = lines.size();
  const size_t cols = lines[0].size();
  std::queue<Beam> beams;
  beams.push(initalBeam);
  while (!beams.empty()) {
    auto &b = beams.front();
    b.coords += b.dir;
    if (b.coords.row >= 0 && b.coords.row < rows && b.coords.col >= 0 &&
        b.coords.col < cols) {
      auto &beamDirection = beamDirections[b.coords.row][b.coords.col];
      // Log beam direction.
      const char bd = static_cast<char>(b.dirBF());
      if ((beamDirection & bd) != 0) {
        // Already been at this cell with this direction.
        beams.pop();
        continue;
      } else {
        beamDirection = beamDirection | bd;
      }
      // Handle the cell content.
      const char ch = lines[b.coords.row][b.coords.col];
      if (ch == '/') {
        if (b.dir.col > 0) {
          b.dir = Coords::Up();
        } else if (b.dir.col < 0) {
          b.dir = Coords::Down();
        } else if (b.dir.row > 0) {
          b.dir = Coords::Left();
        } else if (b.dir.row < 0) {
          b.dir = Coords::Right();
        }
      } else if (ch == '\\') {
        if (b.dir.col > 0) {
          b.dir = Coords::Down();
        } else if (b.dir.col < 0) {
          b.dir = Coords::Up();
        } else if (b.dir.row > 0) {
          b.dir = Coords::Right();
        } else if (b.dir.row < 0) {
          b.dir = Coords::Left();
        }
      } else if (ch == '|') {
        if (b.dir.col != 0) {
          // Split.
          b.dir = Coords::Up();
          beams.emplace(b.coords, Coords::Down());
        }
      } else if (ch == '-') {
        if (b.dir.row != 0) {
          // Split.
          b.dir = Coords::Left();
          beams.emplace(b.coords, Coords::Right());
        }
      }
    } else {
      // Out of bounds.
      beams.pop();
    }
    // std::cout << "\nnext:\n";
    // printBeamDirections(beamDirections);
  }
  // std::cout << "\nfinal:\n";
  // printBeamDirections(beamDirections);
  return countLit(beamDirections);
}

void solvePart1(const std::vector<std::string> &lines) {
  const auto lit = simulateBeam(lines, Beam(Coords(0, -1), Coords(0, 1)));
  std::cout << "Lit: " << lit << "\n";
}

void solvePart2(const std::vector<std::string> &lines) {
  // Top, bottom
  const size_t rows = lines.size();
  const size_t cols = lines[0].size();
  size_t maxLit = 0;
  for (size_t col = 0; col < cols; ++col) {
    maxLit = std::max(maxLit,
                      simulateBeam(lines, Beam(Coords(-1, col), Coords(1, 0))));
    maxLit = std::max(
        maxLit, simulateBeam(lines, Beam(Coords(rows, col), Coords(-1, 0))));
  }
  for (size_t row = 0; row < rows; ++row) {
    maxLit = std::max(maxLit,
                      simulateBeam(lines, Beam(Coords(row, -1), Coords(0, 1))));
    maxLit = std::max(
        maxLit, simulateBeam(lines, Beam(Coords(row, cols), Coords(0, -1))));
  }
  std::cout << "Max. lit: " << maxLit << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  std::vector<std::string> lines;
  std::vector<std::vector<char>> lightDirs;
  std::string line;
  while (std::getline(ifs, line)) {
    lines.push_back(line);
  }
  measureTime([&lines]() { solvePart1(lines); }, "Part 1");
  measureTime([&lines]() { solvePart2(lines); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}