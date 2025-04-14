#include <string>
#include <vector>

using namespace std;

namespace MatchSimK {
  using interval = tuple<int, int>;               // [start, end]
  using triple = tuple<interval, interval, int>;  // ([f_1, f_2], [b_1, b_2], offset)

  vector<triple> matchSimK(string text, string pattern, int k);
}