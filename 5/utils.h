#include <chrono>
#include <iomanip>
#include <iostream>

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
