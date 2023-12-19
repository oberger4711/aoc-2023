#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <string>

// Print found path through the maze. Requires more memory and slows down.
//#define HISTORY = 1

static inline int heuristic(const Coords &start, const Coords &dest) {
  const auto diff = dest - start;
  return diff.manhattanDistance();
}

const char UP = 0;
const char LEFT = 1;
const char DOWN = 2;
const char RIGHT = 3;
const char NONE = 4;

char dirToArrow(char dir) {
  char ret = '?';
  if (dir == UP) {
    ret = '^';
  } else if (dir == LEFT) {
    ret = '<';
  } else if (dir == DOWN) {
    ret = 'v';
  } else if (dir == RIGHT) {
    ret = '>';
  }
  return ret;
}

struct Node {
  Coords coords;
  char lastDir = NONE;
  unsigned char lastDirCount = 0;
  int costsPast = 0;
  int costsFuturePredicted;
#ifdef HISTORY
  std::string history = "";
#endif

  Node(const Coords &coords_, char lastDir_, unsigned char lastDirCount_,
       const Coords &tr_)
      : coords(coords_), lastDir(lastDir_), lastDirCount(lastDirCount_),
        costsFuturePredicted(heuristic(coords_, tr_)) {}

  void addDir(char dir) {
    if (lastDir == dir) {
      ++lastDirCount;
    } else {
      lastDir = dir;
      lastDirCount = 1;
    }
  }

  inline int totalPredictedCosts() const {
    return costsPast + costsFuturePredicted;
  }
};

bool operator<(const Node &lhs, const Node &rhs) {
  return -(lhs.totalPredictedCosts()) < -(rhs.totalPredictedCosts());
}

inline int costs(const std::vector<std::string> &lines, const Coords &coords) {
  return lines[coords.row][coords.col] - '0';
}

template <typename MoveConstraints> struct AStar {
  AStar(const std::vector<std::string> &lines_)
      : lines(lines_), moveConstraints(), rows(lines.size()),
        cols(lines[0].size()), dest(rows - 1, cols - 1) {}

  int solve() const {
    const Coords up(Coords::Up());
    const Coords left(Coords::Left());
    const Coords down(Coords::Down());
    const Coords right(Coords::Right());

    std::vector<char> blackList(rows * cols * 40, 0);
    // std::cout << "hashset size: " << blackList.size() << "\n";
    std::priority_queue<Node> q;
    Coords start(0, 0);
    q.emplace(start, NONE, 0, dest);
    int finalCosts = std::numeric_limits<int>::max();
    bool reachedDest = false;
    while (!q.empty() && !reachedDest) {
      const Node n = q.top();
      q.pop();
      // if (n.history ==
      //    std::string(">>>>>>>>vvvv>>>>vvvvvvvv").substr(0, n.history.size()))
      //    {
      //  std::cout << "Expanding " << n.coords << " with costs "
      //            << n.totalPredictedCosts() << ", hist: " << n.history <<
      //            "\n";
      //}
      if (n.coords == dest && moveConstraints.canStop(n)) {
        finalCosts = n.costsPast;
        reachedDest = true;
#ifdef HISTORY
        std::cout << n.history << "\n";
#endif
      }
      // Expand
      if (n.coords.row > 0) {
        tryExpandAndPush(n, UP, up, q, blackList);
      }
      if (n.coords.row < rows - 1) {
        tryExpandAndPush(n, DOWN, down, q, blackList);
      }
      if (n.coords.col > 0) {
        tryExpandAndPush(n, LEFT, left, q, blackList);
      }
      if (n.coords.col < cols - 1) {
        tryExpandAndPush(n, RIGHT, right, q, blackList);
      }
    }
    return finalCosts;
  }

  inline bool tryExpandAndPush(const Node &base, char dir,
                               const Coords &dirCoords,
                               std::priority_queue<Node> &q,
                               std::vector<char> &blackList) const {
    bool ret = false;
    if (moveConstraints.canMove(base, dir)) {
      const Coords newCoords = base.coords + dirCoords;
      const auto en = expand(base, dir, newCoords);
      const auto enCosts = en.totalPredictedCosts();
      auto &alreadyEnqueuedOrProcessedCosts = blackList[hash(en)];
      if (alreadyEnqueuedOrProcessedCosts == 0 ||
          enCosts < alreadyEnqueuedOrProcessedCosts) {
        // if (print) {
        //  std::cout << "dir: " << dirToArrow(dir) << ", ";
        //  std::cout << "history: " << en.history << ", ";
        //  std::cout << "hash: " << hash(en) << ", ";
        //  std::cout << "en costs: " << enCosts << "\n";
        //}
        q.emplace(std::move(en));
        alreadyEnqueuedOrProcessedCosts = enCosts;
        ret = true;
      }
    }
    return ret;
  }

  inline Node expand(const Node &base, char dir,
                     const Coords &newCoords) const {
    Node en(base);
    en.coords = newCoords;
    en.addDir(dir);
    en.costsPast += costs(lines, newCoords);
    en.costsFuturePredicted = heuristic(newCoords, dest);
#ifdef HISTORY
    en.history += dirToArrow(dir);
#endif
    return en;
  }

  inline size_t hash(const Node &node) const {
    assert(node.lastDir < 4);
    assert(node.lastDirCount > 0);
    assert(node.lastDirCount <= 10);
    return static_cast<size_t>(node.coords.row * cols + node.coords.col) * 4 *
               10 +
           static_cast<size_t>(node.lastDir * 10 + node.lastDirCount - 1);
  }

  const std::vector<std::string> &lines;
  const MoveConstraints moveConstraints;
  int rows;
  int cols;
  Coords dest;
};

struct MoveConstraints1 {
  bool canMove(const Node &n, char dir) const {
    const bool initialMove = (n.lastDir == NONE);
    const bool reverse = (dir == UP && n.lastDir == DOWN) ||
                         (dir == LEFT && n.lastDir == RIGHT) ||
                         (dir == DOWN && n.lastDir == UP) ||
                         (dir == RIGHT && n.lastDir == LEFT);
    return initialMove ||
           (!reverse && (dir != n.lastDir || n.lastDirCount < 3));
  }

  bool canStop(const Node &) const { return true; }
};

struct MoveConstraints2 {
  bool canMove(const Node &n, char dir) const {
    const bool initialMove = (n.lastDir == NONE);
    const bool reverse = (dir == UP && n.lastDir == DOWN) ||
                         (dir == LEFT && n.lastDir == RIGHT) ||
                         (dir == DOWN && n.lastDir == UP) ||
                         (dir == RIGHT && n.lastDir == LEFT);
    const bool allowedChangingOfDirection =
        (dir != n.lastDir && n.lastDirCount >= 4);
    const bool allowedKeepingOfDirection =
        (dir == n.lastDir && n.lastDirCount < 10);
    return initialMove || (!reverse && (allowedKeepingOfDirection ||
                                        allowedChangingOfDirection));
  }

  bool canStop(const Node &n) const { return n.lastDirCount >= 4; }
};

void solvePart1(const std::vector<std::string> &lines) {
  const auto costs = AStar<MoveConstraints1>(lines).solve();
  std::cout << "Costs: " << costs << "\n";
}

void solvePart2(const std::vector<std::string> &lines) {
  const auto costs = AStar<MoveConstraints2>(lines).solve();
  std::cout << "Costs: " << costs << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  // std::ifstream ifs("input_test_2.txt");
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(ifs, line)) {
    lines.push_back(line);
  }
  measureTime([lines]() { solvePart1(lines); }, "Part 1");
  measureTime([lines]() { solvePart2(lines); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}