#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <deque>
#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

#include "state.hpp"
#include "grammar.hpp"

#define DEBUG(x) do { std::cerr << x << endl; } while (0)

using namespace std;

typedef unordered_set<State> chart;
typedef deque<State> worklist;

struct word {
  symbol lhs;
  int origin, loc;

  word(symbol l, int o, int c) : lhs(l), origin(o), loc(c) {}

  inline bool operator==(const struct word &other) const {
    return (lhs == other.lhs
            && origin == other.origin
            && loc == other.loc);
  }
};

struct msg {
  symbol lhs;
  int origin;

  msg(symbol l, int o) : lhs(l), origin(o) {}

  inline bool operator==(const struct msg &other) const {
    return (lhs == other.lhs && origin == other.origin);
  }
};

template <> struct std::hash<struct word> {
  size_t operator()(const struct word& w) const {
    size_t res = 17;
    res = res * 31 + hash<symbol>()(w.lhs);
    res = res * 31 + hash<int>()(w.origin);
    res = res * 31 + hash<int>()(w.loc);
    return res;
  }
};

template <> struct std::hash<struct msg> {
  size_t operator()(const struct msg& r) const {
    size_t res = 17;
    res = res * 31 + hash<symbol>()(r.lhs);
    res = res * 31 + hash<int>()(r.origin);
    return res;
  }
};

void print_chart(const Grammar &grammar, chart &chart) {
  for (const State &s : chart) {
    cout << "(0, ";
    s.print(cout, grammar);
    cout << ")" << endl;
  }
}

inline void insert(State new_state, chart &chart, worklist &worklist) {
  bool did_insert = chart.insert(new_state).second;
  if (did_insert) { worklist.push_back(new_state); }
}

bool parse(const Grammar &grammar, const vector<int> &sentence) {
  chart chart;
  worklist worklist;

  unordered_set<struct word> words;
  unordered_multimap<struct msg, int> replies;
  unordered_multimap<struct msg, State> requests;

  // Insert rules of the form (START -> . a, 0) into C[0].
  for (const rule &r : grammar[Grammar::START_SYMBOL]) {
    insert(State(&r, 0), chart, worklist);
  }

  while (worklist.size() > 0) {
    State state = *worklist.begin();
    worklist.pop_front();

    // cerr << "Popped state: ";
    // state.print(cerr, grammar);
    // cerr << endl;

    if (!state.is_finished()) {
      if (grammar.is_nonterminal(state.next_symbol())) {
        // DEBUG("PREDICT");
        // Predict
        struct msg m (state.next_symbol(), state.loc);
        if (requests.find(m) == requests.end()) {
          for (const rule &r : grammar[state.next_symbol()]) {
            insert(State(&r, state.loc), chart, worklist);
          }
        }
        requests.insert(make_pair(m, state));

        auto range = replies.equal_range(m);
        for (auto it = range.first; it != range.second; ++it) {
          insert(state.incr_pos(it->second), chart, worklist);
        }
      } else {
        // DEBUG("SCAN");
        // Scan
        if (state.loc + 1 <= sentence.size()
            && sentence[state.loc] == state.next_symbol()) {
          insert(state.incr_pos(state.loc + 1), chart, worklist);
        }
      }
    } else {
      // Complete
      // DEBUG("COMPLETE");
      struct word w (state.lhs(), state.origin, state.loc);
      struct msg m (state.lhs(), state.origin);
      bool did_insert = words.insert(w).second;
      if (did_insert) {
        replies.insert(make_pair(m, state.loc));

        auto range = requests.equal_range(m);
        for (auto it = range.first; it != range.second; ++it) {
          insert(it->second.incr_pos(state.loc), chart, worklist);
        }
      }
    }
  }

  print_chart(grammar, chart);
  return false;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Usage: earley GRAMMAR FILE" << endl;
    return 1;
  }

  ifstream grammar_f (argv[1]);
  ifstream words_f (argv[2]);

  Grammar g (grammar_f);

  words_f.seekg(0, std::ios::end);
  size_t size = words_f.tellg();
  std::string buffer(size, ' ');
  words_f.seekg(0);
  words_f.read(&buffer[0], size);
  vector<int> words = g.tokenize(buffer);

  parse(g, words);

  return 0;
}
