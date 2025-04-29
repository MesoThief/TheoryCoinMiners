#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <limits>
#include <vector>

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

struct CheckPoint {
    Interval link;
    std::string partial_shortlex;

    CheckPoint() {};
    CheckPoint(Interval link, std::string partial_shortlex)
        : link(link), partial_shortlex(partial_shortlex) {};
};

constexpr int INF = std::numeric_limits<int>::max();

inline std::ostream& operator<<(std::ostream& os, const Interval& interval) {
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