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

typedef vector<unordered_set<state> > chart;
typedef deque<state> worklist;

inline void insert(int k, state &new_state, chart &chart, worklist &worklist) {
  bool did_insert = chart[k].insert(new_state).second;
  if (did_insert) {
    worklist.push_back(new_state);
    cerr << "Debug: " << new_state << endl;
  }
}

bool parse(const grammar &grammar, const vector<string> &words) {
  chart chart (words.size());
  worklist worklist;

  auto search = grammar.equal_range("START");
  for (auto it = search.first; it != search.second; ++it) {
    state new_state (it);
    insert(0, new_state, chart, worklist);
  }

  for (int k = 0; k < words.size(); k++) {
    while (worklist.size() > 0) {
      state state = *worklist.begin();
      worklist.pop_front();

      if (!finished(state)) {
        if (grammar.find(next_elem(state)) != grammar.end()) {
          cerr << "Debug: Running predictor." << endl;

          auto search = grammar.equal_range(next_elem(state));
          for (auto it = search.first; it != search.second; ++it) {
            struct state new_state (it);
            new_state.origin = k;
            insert(k, new_state, chart, worklist);
          }
        } else {
          if (k + 1 < chart.size()) {
            cerr << "Debug: Running scanner." << endl;

            if (words[k] == next_elem(state)) {
              struct state new_state (state);
              new_state = incr_pos(state);
              insert(k+1, new_state, chart, worklist);
            }
          }
        }
      } else {
        cerr << "Debug: Running completer." << endl;

        for (auto it = chart[state.origin].begin();
             it != chart[state.origin].end(); ++it) {
          if (it->rhs()[it->pos] == state.lhs()) {
            struct state new_state (state);
            new_state = incr_pos(state);
            insert(k, new_state, chart, worklist);
          }
        }
      }
    }
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
