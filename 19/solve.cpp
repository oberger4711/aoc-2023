#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>

struct Item {
  int x, m, a, s;

  size_t sum() const { return x + m + a + s; }
};

struct Range {
  int lower = 1;    // Inclusive
  int upper = 4001; // Exclusive

  size_t count() const {
    return static_cast<size_t>(std::max(0, upper - lower));
  }

  bool isSplitBy(int value) const { return lower < value && value < upper - 1; }
};

struct RangedItem {
  Range x, m, a, s;

  size_t combinations() const {
    return x.count() * m.count() * a.count() * s.count();
  }

  std::optional<std::pair<Range, Range>> splitLT(Range RangedItem::*attr,
                                                 int reference) {
    std::optional<std::pair<Range, Range>> ret;
    const Range &range = this->*attr;
    if (range.isSplitBy(reference)) {
      ret = std::pair<Range, Range>(range, range);
      auto &splitRangeSuccess = ret->first;
      auto &splitRangeFailure = ret->second;
      splitRangeSuccess.upper = reference;
      assert(splitRangeSuccess.lower < splitRangeSuccess.upper);
      splitRangeFailure.lower = reference;
      assert(splitRangeFailure.lower < splitRangeFailure.upper);
    }
    return ret;
  }

  std::optional<std::pair<Range, Range>> splitGT(Range RangedItem::*attr,
                                                 int reference) {
    std::optional<std::pair<Range, Range>> ret;
    const Range &range = this->*attr;
    if (range.isSplitBy(reference)) {
      ret = std::pair<Range, Range>(range, range);
      auto &splitRangeSuccess = ret->first;
      auto &splitRangeFailure = ret->second;
      splitRangeSuccess.lower = reference;
      assert(splitRangeSuccess.lower < splitRangeSuccess.upper);
      splitRangeFailure.upper = reference;
      assert(splitRangeFailure.lower < splitRangeFailure.upper);
    }
    return ret;
  }
};

enum class Action { FORWARD, ACCEPT, REJECT };

class Step {
public:
  // Ctor for steps that always evaluate to success.
  Step() : attr(nullptr), cmp(nullptr), reference(0) {}

  Step(int Item::*attr_, bool (*cmp_)(int, int), int reference_)
      : attr(attr_), cmp(cmp_), reference(reference_) {}

  bool process(const Item &item) const {
    bool ret;
    if (!attr || !cmp || cmp(item.*attr, reference)) {
      if (action == Action::FORWARD) {
        ret = nextSuccess->process(item);
      } else if (action == Action::ACCEPT) {
        ret = true;
      } else if (action == Action::REJECT) {
        ret = false;
      }
    } else {
      ret = nextFailure->process(item);
    }
    return ret;
  }

  void setNextSuccess(const Step *next_) { nextSuccess = next_; }
  void setNextFailure(const Step *next_) { nextFailure = next_; }
  void setAction(Action action_) { action = action_; }

private:
  int Item::*attr;
  bool (*cmp)(int lhs, int rhs);
  int reference;
  Action action{Action::REJECT};
  const Step *nextSuccess{nullptr};
  const Step *nextFailure{nullptr};
};

class RangedStep {
public:
  enum class Comparison { LT, GT, NONE };
  // Ctor for steps that always evaluate to success.
  RangedStep() : attr(nullptr), cmp(Comparison::NONE), reference(0) {}

  RangedStep(int Item::*attr_, Comparison cmp_, int reference_)
      : attr(attr_), cmp(cmp_), reference(reference_) {}

  size_t process(const RangedItem &rangedItem) const {
    bool ret;
    if (!attr || cmp == Comparison::NONE) {
      if (action == Action::FORWARD) {
        ret = nextSuccess->process(rangedItem);
      } else if (action == Action::ACCEPT) {
        ret = true;
      } else if (action == Action::REJECT) {
        ret = false;
      }
    }
    else {
      std::optional<RangedItem, RangedItem> splitRangedItems;
      if (cmp == Comparison::LT) {
        splitRangedItems = rangedItem.splitLT(attr, reference);
        if (splitRangedItems) {

        }
      }
    }
    return ret;
  }
  // bool process(const Item &item) const {
  //  bool ret;
  //  if (!attr || !cmp || cmp(item.*attr, reference)) {
  //    if (action == Action::FORWARD) {
  //      ret = nextSuccess->process(item);
  //    } else if (action == Action::ACCEPT) {
  //      ret = true;
  //    } else if (action == Action::REJECT) {
  //      ret = false;
  //    }
  //  } else {
  //    ret = nextFailure->process(item);
  //  }
  //  return ret;
  //}

  size_t process(const RangedItem &rangedItem) const {
    bool ret;

    return ret;
  }

  void setNextSuccess(const RangedStep *next_) { nextSuccess = next_; }
  void setNextFailure(const RangedStep *next_) { nextFailure = next_; }
  void setAction(Action action_) { action = action_; }

private:
  int Item::*attr;
  Comparison cmp;
  int reference;
  Action action{Action::REJECT};
  const RangedStep *nextSuccess{nullptr};
  const RangedStep *nextFailure{nullptr};
};

inline std::string parseWorkflowName(const std::string &line, int &pos) {
  const auto epos = line.find('{', pos);
  std::string ret;
  if (epos != std::string::npos) {
    ret = line.substr(pos, epos - pos);
    pos = epos;
  } else {
    throw std::runtime_error("Failed to parse workflow name.");
  }
  return ret;
}

int Item::*chToAttr(char ch) {
  int Item::*ret;
  if (ch == 'x') {
    ret = &Item::x;
  } else if (ch == 'm') {
    ret = &Item::m;
  } else if (ch == 'a') {
    ret = &Item::a;
  } else if (ch == 's') {
    ret = &Item::s;
  }
  return ret;
}

bool compareLT(int lhs, int rhs) { return lhs < rhs; }

bool compareGT(int lhs, int rhs) { return lhs > rhs; }

auto chToCmp(char ch) {
  bool (*ret)(int, int);
  if (ch == '<') {
    ret = &compareLT;
  } else if (ch == '>') {
    ret = &compareGT;
  } else {
    throw std::runtime_error("Failed to parse comparator.");
  }
  return ret;
}

inline bool isStep(const std::string &line, int &pos) {
  const auto cmp = line[pos + 1];
  return cmp == '<' || cmp == '>';
}

// For part 1
inline Step parseStep(const std::string &line, int &pos) {
  // std::cout << line[pos];
  auto attr = chToAttr(line[pos++]);
  // std::cout << " " << line[pos];
  auto cmp = chToCmp(line[pos++]);
  const auto sepPos = line.find(':', pos);
  const int reference = std::stoi(line.substr(pos, sepPos - pos));
  pos = sepPos + 1;
  // std::cout << " " << reference << "\n";
  return Step(attr, cmp, reference);
}

// For part 2
inline Step parseRangedStep(const std::string &line, int &pos) {
  // std::cout << line[pos];
  auto attr = chToAttr(line[pos++]);
  // std::cout << " " << line[pos];
  auto cmp = chToCmp(line[pos++]);
  const auto sepPos = line.find(':', pos);
  const int reference = std::stoi(line.substr(pos, sepPos - pos));
  pos = sepPos + 1;
  // std::cout << " " << reference << "\n";
  return Step(attr, cmp, reference);
}

inline std::string parseDestName(const std::string &line, int &pos) {
  const auto sepPos = std::min(line.size() - 1, line.find(',', pos));
  auto ret = line.substr(pos, sepPos - pos);
  pos = sepPos + 1;
  // std::cout << "dest " << ret << "\n";
  return ret;
}

template <typename T>
std::vector<T> parseSteps(std::ifstream &ifs,
                          T (*parseFunc)(const std::string &, int &),
                          Step *&outStepIn) {
  size_t idxIn = std::numeric_limits<size_t>::max();
  std::string line;
  std::vector<T> steps;
  // For resolution of the next pointers:
  std::unordered_map<std::string, size_t> wfMap;
  std::vector<std::string> nextSuccesses;
  std::string wfName;
  while (std::getline(ifs, line) && !line.empty()) {
    int pos = 0;
    wfName = parseWorkflowName(line, pos);
    if (wfName == "in") {
      idxIn = steps.size();
    }
    ++pos; // Skip {
    wfMap[wfName] = steps.size();
    assert(pos < line.size() - 1);
    const auto ePos = line.size() - 1;
    // std::cout << "Workflow " << wfName << "\n";
    while (pos < line.size() - 1) {
      if (isStep(line, pos)) {
        steps.push_back(parseFunc(line, pos));
        nextSuccesses.push_back(parseDestName(line, pos));
      } else {
        steps.emplace_back();
        nextSuccesses.push_back(parseDestName(line, pos));
      }
    }
    // Now that the steps are constructed and their address stays fixed, resolve
    // the next steps and set the pointers.
    for (size_t i = 0; i < steps.size(); ++i) {
      auto &step = steps[i];
      step.setNextFailure(&steps[i + 1]);
      const auto &success = nextSuccesses[i];
      if (success == "A") {
        step.setAction(Action::ACCEPT);
      } else if (success == "R") {
        step.setAction(Action::REJECT);
      } else {
        step.setAction(Action::FORWARD);
        step.setNextSuccess(&steps[wfMap[success]]);
      }
    }
  }
  assert(idxIn !=
         std::numeric_limits<size_t>::max()); // Should have found "in" by now.
  outStepIn = &(steps[idxIn]);
  return steps;
}

Item parseItem(const std::string &line) {
  Item ret;
  int pos = 1; // Skip {
  while (pos < line.size() - 1) {
    const auto attr = chToAttr(line[pos]);
    pos += 2; // Skip =
    const auto commaPos = std::min(line.size() - 1, line.find(',', pos));
    const auto valueStr = line.substr(pos, commaPos - pos);
    // std::cout << valueStr << "\n";
    const int value = std::stoi(valueStr);
    pos = commaPos + 1;
    ret.*attr = value;
  }
  return ret;
}

void solvePart1() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  Step *stepIn = nullptr;
  const auto steps = parseSteps(ifs, &parseStep, stepIn);
  assert(stepIn != nullptr);
  std::string line;
  size_t sum = 0;
  while (std::getline(ifs, line)) {
    Item item = parseItem(line);
    if (stepIn->process(item)) {
      sum += item.sum();
    }
  }
  std::cout << "Sum: " << sum << "\n";
}

void solvePart2() {
  // TODO
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}