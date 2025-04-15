#ifndef COMMON_H
#define COMMON_H

#include <limits>

struct Interval {
  int start;
  int end;

  Interval() {};
  Interval(int start, int end) : start(start), end(end) {};
};

constexpr int INF = std::numeric_limits<int>::max();

#endif  // COMMON_H