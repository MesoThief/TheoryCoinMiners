#include <string>
#include <vector>

#include "utils/Common.h"

using namespace std;

namespace MatchSimK {
  using triple = tuple<Interval, Interval, int>;  // ([f_1, f_2], [b_1, b_2], offset)

  vector<triple> matchSimK(string text, string pattern, int k);
}