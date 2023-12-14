#include "utils.h"
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> parseField(std::ifstream &ifs) {
  std::string line;
  std::vector<std::string> field;
  while (std::getline(ifs, line)) {
    field.push_back(line);
  }
  return field;
}

template <int dir> size_t rollVertical(std::vector<std::string> &field) {
  size_t totalLoad = 0;
  const int rows = static_cast<int>(field.size());
  const int cols = static_cast<int>(field[0].size());
  const int rowStart = (dir < 0) ? 0 : rows - 1;
  auto checkRowInBounds = [rows](int row) {
    return (dir < 0) ? (row < rows) : (row >= 0);
  };
  constexpr int rowInc = (dir < 0) ? 1 : -1;
  for (int row = rowStart; checkRowInBounds(row); row += rowInc) {
    auto &line = field[row];
    for (int col = 0; col < cols; ++col) {
      if (line[col] == 'O') {
        int maxAllowedDist = 0;
        for (int dist = dir;
             (dist < 0 && dist >= -row) || (dist > 0 && dist < (rows - row));
             dist += dir) {
          if (field[row + dist][col] == '.') {
            maxAllowedDist = dist;
          } else {
            break;
          }
        }
        const int destRow = row + maxAllowedDist;
        if (maxAllowedDist != 0) {
          // Move the round rock.
          line[col] = '.';
          field[row + maxAllowedDist][col] = 'O';
        }
        totalLoad += rows - destRow;
      }
    }
  }
  return totalLoad;
}

template <int dir> size_t rollHorizontal(std::vector<std::string> &field) {
  size_t totalLoad = 0;
  const int rows = static_cast<int>(field.size());
  const int cols = static_cast<int>(field[0].size());
  const int colStart = (dir < 0) ? 0 : cols - 1;
  auto checkColInBounds = [cols](int col) {
    return (dir < 0) ? (col < cols) : (col >= 0);
  };
  constexpr int colInc = (dir < 0) ? 1 : -1;
  for (int col = colStart; checkColInBounds(col); col += colInc) {
    for (int row = 0; row < rows; ++row) {
      auto &line = field[row];
      if (line[col] == 'O') {
        int maxAllowedDist = 0;
        for (int dist = dir;
             (dist < 0 && dist >= -col) || (dist > 0 && dist < (cols - col));
             dist += dir) {
          if (field[row][col + dist] == '.') {
            maxAllowedDist = dist;
          } else {
            break;
          }
        }
        const int destCol = col + maxAllowedDist;
        if (maxAllowedDist != 0) {
          // Move the round rock.
          line[col] = '.';
          line[col + maxAllowedDist] = 'O';
        }
        totalLoad += rows - row;
      }
    }
  }
  return totalLoad;
}

void solvePart1() {
  std::ifstream ifs("input.txt");
  auto field = parseField(ifs);
  const auto totalLoad = rollVertical<-1>(field);
  // std::cout << field;
  assert(totalLoad == 109654);
  std::cout << "Total load: " << totalLoad << "\n";
}

template <size_t size> inline bool allEqual(const std::array<size_t, size> &a) {
  const auto ref = a[0];
  bool equal = true;
  for (size_t i = 1; i < size && equal; ++i) {
    equal = (a[i] = ref);
  }
  return equal;
}

void solvePart2() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  auto field = parseField(ifs);
  size_t totalLoad = 0;
  size_t lastInsertIndex = 0;
  for (size_t i = 0; i < 12498; ++i) {
    rollVertical<-1>(field);
    rollHorizontal<-1>(field);
    rollVertical<1>(field);
    totalLoad = rollHorizontal<1>(field);
    if (i > 12464) {
      std::cout << i << ": " << totalLoad << "\n";
    }
  }
  // Now I detected the repetition pattern manually.
  // I saw a pattern starting at i = 12469 and ending at i = 12496.
  // Assuming that it would repeat on and on until 1000000000,
  // I can compute the i for which I get the equivalent result as for
  // 1000000000. i_final = 12469 + ((1000000000 - 1 - 12469) % (12496 - 12469))
  //         = 12474
  // The total load for this i was 94876.
  std::cout << "Final total load: " << 94876 << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}
