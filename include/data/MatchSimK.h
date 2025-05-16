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

        // This constructor is for saving XY-link
        CheckPoint(Interval link, std::string partial_shortlex)
            : link(link), partial_shortlex(partial_shortlex) {};

        // This constructor is for saving YX-link
        CheckPoint(Interval link, std::string partial_shortlex, std::vector<int> x_vector, std::vector<int> y_vector)
            : link(link), partial_shortlex(partial_shortlex), x_vector(x_vector), y_vector(y_vector) {};
    };

    string shortlex_with_checkpoint(
        int k,
        int pattern_universality,
        const string& sub_T_string,
        vector<vector<MatchSimK::CheckPoint>>& check_points,
        const vector<int>& x_arch_indexes,
        vector<int>& y_arch_indexes,
        bool isUniversalPattern
    );
}  // namespace MatchSimK