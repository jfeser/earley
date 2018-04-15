#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <deque>
#include <string>
#include <map>
#include <utility>
#include <vector>

#include "tbb/concurrent_unordered_set.h"
#include "tbb/parallel_do.h"

#include "earley_shared.hpp"

size_t tbb_hasher(const state& s) {
  // Compute individual hash values for first, second and third
  // http://stackoverflow.com/a/1646913/126995
  size_t res = 17;
  res = res * 31 + std::hash<void * >()((void *)(&*s.rule));
  res = res * 31 + std::hash<int>()(s.pos);
  res = res * 31 + std::hash<int>()(s.origin);
  return res;
}

typedef vector<tbb::concurrent_unordered_set<state> > chart;

struct EarleyBody {
  const grammar &grammar;
  const vector<string> &words;
  const int k;
  chart &chart;

  EarleyBody(const ::grammar &g, ::chart &c, const vector<string> &w, const int k) :
    grammar(g), words(w), k(k), chart(c) {}

  inline void insert(int j, state new_state, tbb::parallel_do_feeder<state>& feeder) const {
    bool did_insert = chart[j].insert(new_state).second;
    if (did_insert) {
      // Only feed the current loop.
      if (j == k) { feeder.add(new_state); }
    }
  }

  void operator()(state st, tbb::parallel_do_feeder<state>& feeder) const {
    if (!finished(st)) {
      if (grammar.find(next_elem(st)) != grammar.end()) {
        auto search = grammar.equal_range(next_elem(st));
        for (auto it = search.first; it != search.second; ++it) {
          struct state new_state (it);
          new_state.origin = k;
          insert(k, new_state, feeder);
        }
      } else {
        if (k + 1 < chart.size()) {
          if (words[k] == next_elem(st)) {
            insert(k+1, incr_pos(st), feeder);
          }
        }
      }
    } else {
      for (auto s = chart[st.origin].begin();
           s != chart[st.origin].end(); ++s) {
        if (s->rhs()[s->pos] == st.lhs()) {
          insert(k, incr_pos(*s), feeder);
        }
      }
    }
  }
};

bool parse(const grammar &grammar, const vector<string> &words) {
  chart chart (words.size());

  auto search = grammar.equal_range("START");
  for (auto it = search.first; it != search.second; ++it) {
    chart[0].insert(it);
  }

  for (int k = 0; k < words.size(); k++) {
    tbb::parallel_do(chart[k].begin(), chart[k].end(),
                     EarleyBody(grammar, chart, words, k));
  }

  return false;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Usage: earley GRAMMAR FILE" << endl;
    return 1;
  }

  ifstream grammar_f (argv[1]);
  ifstream words_f (argv[2]);
  vector<string> words = split(words_f);

  const grammar grammar = parse_grammar(grammar_f);

  cerr << "Debug: Beginning parse." << endl;
  parse(grammar, words);
  cerr << "Debug: Finished parse." << endl;

  return 0;
}
