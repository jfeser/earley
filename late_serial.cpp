#include "late_serial.hpp"

#include <iostream>

#include <string>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

#include "tbb/tick_count.h"

#include "state.hpp"
#include "grammar.hpp"
#include "late_util.hpp"

using namespace std;

typedef unordered_set<State> chart;
typedef deque<State> worklist;

void LateSerialParser::print_chart() {
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

void LateSerialParser::parse() {
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
}

string LateSerialParser::name() {
  return "late_serial";
}

bool LateSerialParser::is_parallel() {
  return false;
}

void LateSerialParser::reset() {
  chart.clear();
}
