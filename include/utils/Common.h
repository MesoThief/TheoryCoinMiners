#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <limits>
#include <utility>

using namespace std;

#ifdef DEBUG
#define debug(x) x;
#else
#define debug(x)
#endif

struct Interval {
    int start;
    int end;

    Interval() {}
    Interval(int start, int end) : start(start), end(end) {}
};

constexpr int INF = numeric_limits<int>::max();

inline ostream& operator<<(ostream& os, const Interval& interval) {
    if (interval.start == INF)
        os << "[INF";
    else
        os << "[" << interval.start;

    if (interval.end == INF)
        os << ", INF]";
    else
        os << ", " << interval.end << "]";

    return os;
}

#endif  // COMMON_H