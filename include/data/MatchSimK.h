#include <string>
#include <vector>

#include "utils/Common.h"

using namespace std;

namespace MatchSimK {
    using triple = tuple<Interval, Interval, int>;  // ([f_1, f_2], [b_1, b_2], offset)

    vector<triple> matchSimK(const string& text, const string& pattern, int k);

    struct CheckPoint {
        Interval link;
        std::string partial_shortlex;
        std::vector<int> x_vector, y_vector;

        CheckPoint() {};
        CheckPoint(Interval link, std::string partial_shortlex)
            : link(link), partial_shortlex(partial_shortlex) {};
        CheckPoint(Interval link, std::string partial_shortlex, std::vector<int> x_vector, std::vector<int> y_vector)
            : link(link), partial_shortlex(partial_shortlex), x_vector(x_vector), y_vector(y_vector) {};
    };
}  // namespace MatchSimK