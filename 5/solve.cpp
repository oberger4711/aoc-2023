#include "utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

using num = long long;

// Data structure like a doubly-linked list inside a std::vector.
// O(1) push_back
// O(1) erasing random elements
// Vector as underlying container -> few allocations
template <typename T> class LinkedVector {
public:
  static constexpr size_t npos = static_cast<size_t>(-1);
  struct Element {
    size_t prev;
    size_t next;
    T value;
    template <typename... Args>
    Element(size_t prev_, const T &value_)
        : prev(prev_), next(npos), value(value_) {}
  };

  void reserve(size_t size) { data.reserve(size); }

  size_t first() const { return head; }

  Element &operator[](size_t i) { return data[i]; }
  Element &back() { return data[tail]; }

  void push_back(const T &value) {
    data.emplace_back(tail, value);
    size_t last = data.size() - 1;
    if (size > 0) {
      data[tail].next = last;
    } else {
      head = last;
    }
    tail = last;
    ++size;
  }

  void erase(Element &element) {
    if (element.next != npos) {
      data[element.next].prev = element.prev;
    } else {
      // Last element
      tail = element.prev;
    }
    if (element.prev != npos) {
      data[element.prev].next = element.next;
    } else {
      // First element
      head = element.next;
    }
    --size;
  }

  void clear() {
    data.clear();
    head = npos;
    tail = npos;
    size = 0;
  }

  size_t count() { return size; }
  bool empty() { return size == 0; }

private:
  std::vector<Element> data;
  size_t head = npos;
  size_t tail = npos;
  size_t size = 0;
};

inline std::vector<num> parseNumList(const std::string &line, size_t start) {
  std::vector<num> seeds;
  std::stringstream ss(&line.c_str()[7]);
  while (!ss.eof()) {
    num seed;
    ss >> seed;
    seeds.emplace_back(seed);
  }
  return seeds;
}

// For part 2
struct Range {
  num start;
  num length;
  Range(num start_, num length_) : start(start_), length(length_) {
    if (length_ == 0) {
      throw std::runtime_error("zero length");
    }
  }
};

std::istream &operator>>(std::istream &is, Range &range) {
  is >> range.start >> range.length;
  return is;
}

bool operator<(const Range &lhs, const Range &rhs) {
  return lhs.start < rhs.start;
}

inline LinkedVector<Range> parseRangeList(const std::string &line,
                                          size_t start) {
  LinkedVector<Range> seeds;
  std::stringstream ss(&line.c_str()[7]);
  while (!ss.eof()) {
    num start, length;
    ss >> start >> length;
    seeds.push_back(Range(start, length));
  }
  return seeds;
}

struct Map {
  num destStart, srcStart, length;
  Map() = default;
  Map(num destStart_, num srcStart_, num length_)
      : destStart(destStart_), srcStart(srcStart_), length(length_) {}

  inline bool operator()(num src, num &outMappedValue) {
    const num offset = src - srcStart;
    const bool inside = offset > 0 && offset < length;
    if (inside) {
      outMappedValue = destStart + offset;
    }
    return inside;
  }

  inline void operator()(size_t indexSrc, LinkedVector<Range> &srcs,
                         LinkedVector<Range> &dests) {
    const num mapLeft = srcStart;
    const num mapRight = srcStart + length - 1;
    auto &src = srcs[indexSrc];
    const num rangeLeft = src.value.start;
    const num rangeRight = src.value.start + src.value.length - 1;
    const num clampedRangeLeft = std::clamp(rangeLeft, mapLeft, mapRight);
    const num clampedRangeRight = std::clamp(rangeRight, mapLeft, mapRight);
    if (clampedRangeLeft != clampedRangeRight) {
      // There is overlap.
      srcs.erase(src);
      if (rangeLeft < clampedRangeLeft) {
        // Part of the range is below the map.
        srcs.push_back(Range(rangeLeft, clampedRangeLeft - rangeLeft));
        // src may be invalid from here on.
      }
      if (rangeRight > clampedRangeRight) {
        // Part of the range is above the map.
        srcs.push_back(
            Range(clampedRangeRight + 1, rangeRight - clampedRangeRight));
      }
      // Part inside the map.
      dests.push_back(Range(destStart + (clampedRangeLeft - srcStart),
                            clampedRangeRight + 1 - clampedRangeLeft));
    }
  }
};

std::istream &operator>>(std::istream &is, Map &map) {
  is >> map.destStart >> map.srcStart >> map.length;
  return is;
}

void solvePart1() {
  // std::ifstream is("input_test.txt");
  std::ifstream is("input.txt");
  std::string line;
  std::getline(is, line);
  std::vector<num> cur;
  std::vector<num> next = parseNumList(line, 7);
  std::getline(is, line);
  Map map;
  num offset;
  num mapped;
  while (is) {
    std::swap(cur, next);
    next = cur;
    std::getline(is, line); // x-to-y map
    while (std::getline(is, line) && !line.empty()) {
      std::stringstream ss(line);
      ss >> map;
      for (size_t i = 0; i < cur.size(); ++i) {
        num c = cur[i];
        if (map(c, mapped)) {
          next[i] = mapped;
        }
      }
    }
  }
  num lowest = *std::min_element(next.begin(), next.end());
  std::cout << lowest << "\n";
}

void printRanges(LinkedVector<Range> ranges) {
  std::cout << "\nranges:\n";
  for (auto i = ranges.first(); i != LinkedVector<Range>::npos;
       i = ranges[i].next) {
    std::cout << "  " << ranges[i].value.start << ", len "
              << ranges[i].value.length << "\n";
  }
}

void solvePart2() {
  // std::ifstream is("input_test.txt");
  std::ifstream is("input.txt");
  std::string line;
  std::getline(is, line);
  LinkedVector<Range> cur;
  auto next = parseRangeList(line, 7);
  std::getline(is, line);
  Map map;
  num offset;
  num mapped;
  while (is) {
    std::swap(cur, next);
    next.clear();
    // printRanges(cur);
    std::getline(is, line); // x-to-y map
    while (std::getline(is, line) && !line.empty()) {
      std::stringstream ss(line);
      ss >> map;
      // std::cout << map.srcStart << ", len " << map.length << " ---> "
      //          << map.destStart << ", len " << map.length << ": "
      //          << cur.count() << "x\n";
      for (auto c = cur.first(); c != LinkedVector<Range>::npos;
           c = cur[c].next) {
        map(c, cur, next);
      }
    }
    // Copy everything over that is not yet mapped now.
    for (auto c = cur.first(); c != LinkedVector<Range>::npos;
         c = cur[c].next) {
      next.push_back(cur[c].value);
    }
  }
  auto &locations = next;
  num lowestLocation = std::numeric_limits<num>::max();
  for (auto l = locations.first(); l != LinkedVector<Range>::npos;
       l = locations[l].next) {
    lowestLocation = std::min(lowestLocation, locations[l].value.start);
  }
  std::cout << lowestLocation << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}