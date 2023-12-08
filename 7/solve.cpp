#include "utils.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using ValueArray = std::array<char, 5>;

// Part 1
ValueArray stringToValueArray1(const std::string &s) {
  ValueArray values;
  for (size_t i = 0; i < s.size(); ++i) {
    const char ch = s[i];
    if (ch <= '9') {
      // Number
      values[i] = ch - '2';
    } else if (ch == 'T') {
      values[i] = 8;
    } else if (ch == 'J') {
      values[i] = 9;
    } else if (ch == 'Q') {
      values[i] = 10;
    } else if (ch == 'K') {
      values[i] = 11;
    } else if (ch == 'A') {
      values[i] = 12;
    }
  }
  return values;
}

struct Hand1 {
  static inline void updateType(int sameCount, int &highestTypeValue) {
    int value = 10 * sameCount;
    if (value == 20) {
      if (highestTypeValue == 20) {
        highestTypeValue = 25; // Two pair > one pair
      } else if (highestTypeValue == 30) {
        highestTypeValue = 35; // Full house
      } else {
        highestTypeValue = std::max(highestTypeValue, value);
      }
    } else if (value == 30) {
      if (highestTypeValue == 20) {
        highestTypeValue = 35; // Full house
      } else {
        highestTypeValue = std::max(highestTypeValue, value);
      }
    } else {
      highestTypeValue = std::max(highestTypeValue, value);
    }
  }

  Hand1(const ValueArray &values, short int bid_) : bid(bid_) {
    ValueArray sorted = values;
    std::sort(sorted.begin(), sorted.end());
    int sameCount = 1;
    for (size_t i = 1; i < sorted.size(); ++i) {
      if (sorted[i] == sorted[i - 1]) {
        ++sameCount;
      } else {
        updateType(sameCount, typeValue);
        sameCount = 1;
      }
    }
    // Last card
    updateType(sameCount, typeValue);
    for (size_t i = 0; i < values.size(); ++i) {
      cardValue = cardValue * 13 + values[i];
    }
  }
  int typeValue = 0;
  unsigned int cardValue = 0;
  short int bid;
};

bool operator<(const Hand1 &lhs, const Hand1 &rhs) {
  bool ret;
  if (lhs.typeValue == rhs.typeValue) {
    ret = lhs.cardValue < rhs.cardValue;
  } else {
    ret = lhs.typeValue < rhs.typeValue;
  }
  return ret;
}

void solvePart1() {
  // std::ifstream ifs("input_test.txt");
  std::ifstream ifs("input.txt");
  std::vector<Hand1> hands;
  hands.reserve(1000);
  std::string line;
  std::string valuesString;
  short int bid;
  while (std::getline(ifs, line)) {
    std::istringstream iss(line);
    iss >> valuesString >> bid;
    hands.emplace_back(stringToValueArray1(valuesString), bid);
  }
  std::sort(hands.begin(), hands.end());
  int sum = 0;
  for (size_t i = 0; i < hands.size(); ++i) {
    sum += (i + 1) * hands[i].bid;
    // std::cout << hands[i].typeValue << ", " << hands[i].cardValue << " -> "
    // << hands[i].bid << "\n";
  }
  std::cout << "Sum: " << sum << "\n";
}

// Part 2
constexpr char JOKER_2 = 0;

ValueArray stringToValueArray2(const std::string &s) {
  // Unlike in the first part JOKER is now the weakest!
  ValueArray values;
  for (size_t i = 0; i < s.size(); ++i) {
    const char ch = s[i];
    if (ch <= '9') {
      // Number
      values[i] = ch - '1';
    } else if (ch == 'T') {
      values[i] = 9;
    } else if (ch == 'Q') {
      values[i] = 10;
    } else if (ch == 'K') {
      values[i] = 11;
    } else if (ch == 'A') {
      values[i] = 12;
    } else if (ch == 'J') {
      values[i] = JOKER_2;
    }
  }
  return values;
}

struct Hand2 {
  static inline void updateType(int sameCount, int &highestTypeValue) {
    int value = 10 * sameCount;
    if (value == 20) {
      if (highestTypeValue == 20) {
        highestTypeValue = 25; // Two pair > one pair
      } else if (highestTypeValue == 30) {
        highestTypeValue = 35; // Full house
      } else {
        highestTypeValue = std::max(highestTypeValue, value);
      }
    } else if (value == 30) {
      if (highestTypeValue == 20) {
        highestTypeValue = 35; // Full house
      } else {
        highestTypeValue = std::max(highestTypeValue, value);
      }
    } else {
      highestTypeValue = std::max(highestTypeValue, value);
    }
  }

  static inline void upgradeType(int &typeValue) {
    if (typeValue == 35) {
      typeValue = 40;
    } else {
      typeValue += 10;
    }
  }

  Hand2(const ValueArray &values, short int bid_) : bid(bid_) {
    ValueArray sorted = values;
    std::sort(sorted.begin(), sorted.end());
    int sameCount = 1;
    int jokerCount = 0;
    for (size_t i = 1; i < sorted.size(); ++i) {
      if (sorted[i] == sorted[i - 1]) {
        ++sameCount;
      } else {
        if (sorted[i - 1] == JOKER_2) {
          jokerCount = sameCount;
        } else {
          updateType(sameCount, typeValue);
        }
        sameCount = 1;
      }
    }
    // Last card
    if (sorted.back() == JOKER_2) {
      jokerCount = sameCount;
    } else {
      updateType(sameCount, typeValue);
    }
    for (int i = 0; i < jokerCount; ++i) {
      upgradeType(typeValue);
    }
    for (size_t i = 0; i < values.size(); ++i) {
      cardValue = cardValue * 13 + values[i];
    }
  }
  ValueArray sortedHand;
  int typeValue = 0;
  unsigned int cardValue = 0;
  short int bid;
};

bool operator<(const Hand2 &lhs, const Hand2 &rhs) {
  bool ret;
  if (lhs.typeValue == rhs.typeValue) {
    ret = lhs.cardValue < rhs.cardValue;
  } else {
    ret = lhs.typeValue < rhs.typeValue;
  }
  return ret;
}

void solvePart2() {
  // std::ifstream ifs("input_test.txt");
  //std::ifstream ifs("input_test2.txt");
  std::ifstream ifs("input.txt");
  std::vector<Hand2> hands;
  hands.reserve(1000);
  std::string line;
  std::string valuesString;
  short int bid;
  while (std::getline(ifs, line)) {
    std::istringstream iss(line);
    iss >> valuesString >> bid;
    hands.emplace_back(stringToValueArray2(valuesString), bid);
  }
  std::sort(hands.begin(), hands.end());
  int sum = 0;
  for (size_t i = 0; i < hands.size(); ++i) {
    sum += (i + 1) * hands[i].bid;
    // std::cout << hands[i].typeValue << ", " << hands[i].cardValue << " -> "
    //          << hands[i].bid << "\n";
  }
  std::cout << "Sum: " << sum << "\n";
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}