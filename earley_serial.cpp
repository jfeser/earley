#include "earley_serial.hpp"

#include <iostream>
#include <algorithm>
#include <map>
#include <utility>

using namespace std;

typedef vector<unordered_set<State> > chart_t;
typedef deque<State> worklist_t;

void EarleySerialParser::print_chart() {
  for (int i = 0; i < chart.size(); i++) {
    for (auto s : chart[i]) {
      cout << "(0, ";
      s.print(cout, grammar);
      cout << ")" << endl;
    }
  }
}

inline void insert(int k, State new_state, chart_t &chart, vector<worklist_t> &worklist) {
  bool did_insert = chart[k].insert(new_state).second;
  if (did_insert) {
    worklist[k].push_back(new_state);
  }
}

void EarleySerialParser::parse() {
  chart_t chart (sentence.size() + 1);
  vector<worklist_t> worklist(sentence.size() + 1);

  // Insert rules of the form (START -> . a, 0) into C[0].
  for (const rule &r : grammar[Grammar::START_SYMBOL]) {
    insert(0, State(&r, 0), chart, worklist);
  }

  for (int k = 0; k < sentence.size() + 1; k++) {
    while (worklist[k].size() > 0) {
      State state = *worklist[k].begin();
      worklist[k].pop_front();

      if (!state.is_finished()) {
        symbol next_elem = state.next_symbol();
        if (grammar.is_nonterminal(next_elem)) {
          for (const rule &r : grammar[next_elem]) {
            insert(k, State(&r, k), chart, worklist);
          }
        } else {
          if (k + 1 < chart.size()) {
            if (sentence[k] == next_elem) {
              insert(k+1, state.incr_pos(k+1), chart, worklist);
            }
          }
        }
      } else {
        int &origin = state.origin;
        for (auto s = chart[origin].begin(); s != chart[origin].end(); ++s) {
          if (!s->is_finished() && s->next_symbol() == state.lhs()) {
            insert(k, s->incr_pos(k), chart, worklist);
          }
        }
      }
    }
  }
}

string EarleySerialParser::name() {
  return "earley_serial";
}

bool EarleySerialParser::is_parallel() {
  return false;
}

void EarleySerialParser::reset() {
  chart.clear();
}
