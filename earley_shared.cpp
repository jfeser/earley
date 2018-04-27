#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <deque>
#include <string>
#include <map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "earley_shared.hpp"

using namespace std;

state::state(const grammar::const_iterator rule) :
  rule(rule), pos(0), origin(0) {}

state::state(const state &state) :
  rule(state.rule), pos(state.pos), origin(state.origin) {}

state::state(grammar::iterator &rule, int pos, int origin) :
  rule(rule), pos(pos), origin(origin) {}

bool state::operator==(const state &other) const {
  return (rule == other.rule
          && pos == other.pos
          && origin == other.origin);
}

ostream& operator<<(ostream& os, const state& state) {
  os << "(" << state.lhs() << " -> ";
  for (int i = 0; i < state.rhs().size(); i++) {
    if (state.pos == i) {
      os << "• ";
    }
    os << state.rhs()[i];
    if (i < state.rhs().size() - 1) {
      os << " ";
    }
  }
  os << ", " << state.origin << ")";
  return os;
}

vector<string> split(istream &strm) {
  vector<string> words;
  copy(istream_iterator<string>(strm),
       istream_iterator<string>(),
       back_inserter(words));
  return words;
}

vector<string> split(string &str) {
  istringstream strm(str);
  return split(strm);
}

void dump_vector(vector<string> &v) {
  cerr << "[";
  for (string x : v) {
    cerr << "\"" << x << "\", ";
  }  
  cerr << "]" << endl;
}
