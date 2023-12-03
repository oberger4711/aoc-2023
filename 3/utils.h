#include <chrono>
#include <iomanip>
#include <iostream>

inline bool isDigit(char ch) { return 0x30 <= ch && ch < 0x3A; }

template <typename Func> void measureTime(Func func, const std::string &name) {
  std::cout << "Running " << name << "...\n";
  const auto start = std::chrono::steady_clock::now();
  func();
  const auto end = std::chrono::steady_clock::now();
  const auto diff = end - start;
  // Print duration in readable form.
  std::cout << name << " took ";
  if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() >=
      10000) {
    const auto s =
        std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() /
        1000.0;
    std::cout << std::setprecision(6) << s << " s\n";
  }
  if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() <
      10) {
    const auto ms =
        std::chrono::duration_cast<std::chrono::microseconds>(diff).count() /
        1000.0;
    std::cout << std::setprecision(6) << ms << " ms\n";

  } else {
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
        << " ms\n";
  }
}
