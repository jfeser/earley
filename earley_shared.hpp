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

#ifndef EARLEY_SHARED_H
#define EARLEY_SHARED_H

using namespace std;

typedef multimap<string, vector<string> > grammar;

struct state {
  grammar::const_iterator rule;
  int pos, origin;

  inline const string &lhs() const {
    return this->rule->first;
  }

  inline const vector<string> &rhs() const {
    return this->rule->second;
  }

  state(const grammar::const_iterator rule);
  state(const state &state);
  state(grammar::iterator &rule, int pos, int origin);
  bool operator==(const state &other) const;
};

ostream& operator<<(ostream& os, const state& state);

template <> struct hash<state> {
  size_t operator()(const state& state) const {
    // Compute individual hash values for first, second and third
    // http://stackoverflow.com/a/1646913/126995
    size_t res = 17;
    res = res * 31 + hash<void * >()((void *)(&*state.rule));
    res = res * 31 + hash<int>()(state.pos);
    res = res * 31 + hash<int>()(state.origin);
    return res;
  }
};

inline bool finished(state &state) {
  return state.pos == state.rhs().size();
}

inline const string& next_elem(state &state) {
  return state.rhs()[state.pos];
}

inline state incr_pos(const state &state) {
  struct state new_state = state;
  new_state.pos++;
  return new_state;
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
      }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
      }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

vector<string> split(istream &strm);
vector<string> split(string &str);

void dump_vector(vector<string> &v);

grammar parse_grammar(ifstream &file);

#endif // EARLEY_SHARED_H
