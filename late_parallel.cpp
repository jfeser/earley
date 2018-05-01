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

#include "tbb/concurrent_unordered_set.h"
#include "tbb/concurrent_unordered_map.h"
#include "tbb/parallel_do.h"

#include "state.hpp"
#include "grammar.hpp"
#include "late_util.hpp"

#define DEBUG(x) do { std::cerr << x << endl; } while (0)

using namespace std;

void print_chart(const Grammar &grammar, tbb::concurrent_unordered_set<State> &chart) {
  for (const State &s : chart) {
    cout << "(0, ";
    s.print(cout, grammar);
    cout << ")" << endl;
  }
}

struct LoopBody {
  const Grammar &grammar;
  const vector<symbol> &sentence;
  tbb::concurrent_unordered_set<State> &chart;

  mutable tbb::concurrent_unordered_set<struct word> words;
  mutable tbb::concurrent_unordered_multimap<struct msg, int> replies;
  mutable tbb::concurrent_unordered_multimap<struct msg, State> requests;

  LoopBody(const Grammar &g,
           const vector<symbol> &s,
           tbb::concurrent_unordered_set<State> &c) :
    grammar(g), sentence(s), chart(c) {}

  inline void insert(State new_state, tbb::parallel_do_feeder<State>& feeder) const {
    bool did_insert = chart.insert(new_state).second;
    if (did_insert) { feeder.add(new_state); }
  }

  void operator()(State state, tbb::parallel_do_feeder<State>& feeder) const {
    if (!state.is_finished()) {
      if (grammar.is_nonterminal(state.next_symbol())) {
        // DEBUG("PREDICT");
        // Predict
        struct msg m (state.next_symbol(), state.loc);
        if (requests.find(m) == requests.end()) {
          for (const rule &r : grammar[state.next_symbol()]) {
            insert(State(&r, state.loc), feeder);
          }
        }
        requests.insert(make_pair(m, state));

        auto range = replies.equal_range(m);
        for (auto it = range.first; it != range.second; ++it) {
          insert(state.incr_pos(it->second), feeder);
        }
      } else {
        // DEBUG("SCAN");
        // Scan
        if (state.loc + 1 <= sentence.size()
            && sentence[state.loc] == state.next_symbol()) {
          insert(state.incr_pos(state.loc + 1), feeder);
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
          insert(it->second.incr_pos(state.loc), feeder);
        }
      }
    }
  }
};

bool parse(const Grammar &grammar, const vector<int> &sentence) {
  tbb::concurrent_unordered_set<State> chart;

  // Insert rules of the form (START -> . a, 0) into C[0].
  vector<State> init;
  for (const rule &r : grammar[Grammar::START_SYMBOL]) {
    State s (&r, 0);
    chart.insert(s);
    init.push_back(s);
  }

  tbb::parallel_do(init.begin(), init.end(), LoopBody(grammar, sentence, chart));

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
