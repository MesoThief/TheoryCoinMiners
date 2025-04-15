#ifndef COMMON_H
#define COMMON_H

#include <limits>

#ifdef DEBUG
  #define debug(x) x;
#else
  #define debug(x)
#endif

struct Interval {
  int start;
  int end;

  Interval() {};
  Interval(int start, int end) : start(start), end(end) {};
};

constexpr int INF = std::numeric_limits<int>::max();

#endif  // COMMON_H