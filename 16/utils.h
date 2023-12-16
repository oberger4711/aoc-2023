#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

struct Coords {
  int row, col;
  Coords(int row_, int col_) : row(row_), col(col_) {}
  Coords operator+(const Coords &rhs) const {
    return Coords(row + rhs.row, col + rhs.col);
  }
  Coords operator-(const Coords &rhs) const {
    return Coords(row - rhs.row, col - rhs.col);
  }
  Coords &operator+=(const Coords &rhs) {
    row += rhs.row;
    col += rhs.col;
    return *this;
  }
  Coords &operator-=(const Coords &rhs) {
    row -= rhs.row;
    col -= rhs.col;
    return *this;
  }
  Coords operator*(const int scale) const {
    return Coords(row * scale, col * scale);
  }
  Coords &operator*=(const int scale) {
    row *= scale;
    col *= scale;
    return *this;
  }
  int manhattanDistance() const { return std::abs(row) + std::abs(col); }

  static Coords Up() { return Coords(-1, 0); }
  static Coords Down() { return Coords(1, 0); }
  static Coords Left() { return Coords(0, -1); }
  static Coords Right() { return Coords(0, 1); }
};

inline bool isDigit(char ch) { return 0x30 <= ch && ch < 0x3A; }

template <typename Func>
void measureTime(Func func, const std::string &name, int repetitions = 1) {
  std::cout << "Running " << name << "...\n";
  const auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < repetitions; ++i) {
    func();
  }
  const auto end = std::chrono::steady_clock::now();
  const auto diff = end - start;
  // Print duration in readable form.
  std::cout << name << " took ";
  if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() /
          static_cast<double>(repetitions) >=
      10000.0) {
    const auto s =
        (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() /
         static_cast<double>(repetitions)) /
        1000.0;
    std::cout << std::setprecision(6) << s << " s\n";
  }
  if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() /
          static_cast<double>(repetitions) <
      10.0) {
    const auto ms =
        (std::chrono::duration_cast<std::chrono::microseconds>(diff).count() /
         static_cast<double>(repetitions)) /
        1000.0;
    std::cout << std::setprecision(6) << ms << " ms\n";

  } else {
    std::cout << std::setprecision(6)
              << (std::chrono::duration_cast<std::chrono::milliseconds>(diff)
                      .count() /
                  static_cast<double>(repetitions))
              << " ms\n";
  }
}

inline std::ostream &operator<<(std::ostream &os,
                                const std::vector<std::string> &lines) {
  for (const auto &line : lines) {
    os << line << "\n";
  }
  return os;
}
