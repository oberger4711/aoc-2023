#include "utils.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

inline unsigned char hash(const std::string &s) {
  unsigned char value = 0;
  for (size_t i = 0; i < s.size(); ++i) {
    // std::cout << s[i] << ": (" << static_cast<int>(value) << " + " <<
    // static_cast<int>(s[i]) << ") * 17";
    value = (static_cast<int>(value) + s[i]) * 17;
    // std::cout << " = " << static_cast<int>(value) << "\n";
  }
  return value;
}

std::vector<std::string> parseCSV(std::ifstream &ifs) {
  std::vector<std::string> ret;
  std::string line;
  std::getline(ifs, line);
  std::stringstream iss(line);
  std::string token;
  while (std::getline(iss, token, ',')) {
    ret.push_back(token);
  }
  return ret;
}

void solvePart1() {
  // std::ifstream ifs("input_test.txt");
  std::ifstream ifs("input.txt");
  const auto strs = parseCSV(ifs);
  long long sum = 0;
  for (const auto &s : strs) {
    auto h = hash(s);
    // std::cout << s << ": " << static_cast<int>(h) << "\n";
    sum += h;
  }
  std::cout << "Sum: " << sum << "\n";
}

struct Lens {
  std::string label;
  unsigned short int focalLength;

  Lens(const std::string &label_, unsigned char focalLength_)
      : label(label_), focalLength(focalLength_) {}
};

bool operator==(const Lens &lhs, const std::string &rhs) {
  return lhs.label == rhs;
}

class HashMap {
public:
  void insert(const std::string &label, unsigned char focalLength) {
    const auto h = hash(label);
    auto &box = boxes[h];
    for (auto &l : box) {
      if (l == label) {
        l.focalLength = focalLength;
        return;
      }
    }
    // Not found -> append.
    box.emplace_back(label, focalLength);
  }

  void remove(const std::string &label) {
    const auto h = hash(label);
    auto &box = boxes[h];
    // If the box gets large and we remove often, this may be slow.
    box.erase(std::remove(box.begin(), box.end(), label), box.end());
  }

  size_t focusingPower() {
    size_t ret = 0;
    for (size_t iBox = 0; iBox < boxes.size(); ++iBox) {
      const auto &lenses = boxes[iBox];
      size_t iLens = 0;
      for (const auto &lense : lenses) {
        ret += (iBox + 1) * (iLens + 1) * lense.focalLength;
        ++iLens;
      }
    }
    return ret;
  }

private:
  using Box = std::list<Lens>;
  std::array<Box, 256> boxes;
};

void solvePart2() {
  // std::ifstream ifs("input_test.txt");
  std::ifstream ifs("input.txt");
  const auto strs = parseCSV(ifs);
  HashMap hashMap;
  std::string label;
  for (const auto &s : strs) {
    // std::cout << s << "\n";
    char lastCh = s.back();
    if (lastCh == '-') {
      // - operator
      label = s.substr(0, s.size() - 1);
      hashMap.remove(label);
    } else {
      // = operator
      label = s.substr(0, s.size() - 2);
      const unsigned short focalLength = lastCh - '0';
      hashMap.insert(label, focalLength);
    }
    // std::cout << s << ": " << static_cast<int>(h) << "\n";
  }
  std::cout << "Focusing power: " << hashMap.focusingPower() << "\n";
}

int main() {
  assert(hash("HASH") == 52);
  assert(hash("rn") == 0);
  assert(hash("qp") == 1);
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}
