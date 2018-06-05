// MIT License

// Copyright (c) 2018 Peter Ahrens, John Feser, Robin Hui

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "late_parallel.hpp"

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

using namespace std;

struct LoopBody {
  const Grammar &grammar;
  const vector<symbol> &sentence;
  tbb::concurrent_unordered_set<State> &chart;

  mutable tbb::concurrent_unordered_set<struct word> completed;
  mutable tbb::concurrent_unordered_set<struct msg> started;
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

        bool did_insert = started.insert(m).second;
        requests.insert(make_pair(m, state));
        if (did_insert) {
          for (const rule &r : grammar[state.next_symbol()]) {
            insert(State(&r, state.loc), feeder); // OK
          }
        }

        auto range = replies.equal_range(m);
        for (auto it = range.first; it != range.second; ++it) {
          if (!(it->first == m)) { break; }
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
      bool did_insert = completed.insert(w).second;
      if (did_insert) {
        replies.insert(make_pair(m, state.loc));

        auto range = requests.equal_range(m);
        for (auto it = range.first; it != range.second; ++it) {
          if (!(it->first == m)) { break; }
          insert(it->second.incr_pos(state.loc), feeder);
        }
      }
    }
  }
};

string LateParallelParser::name() {
  return "late_parallel";
}

void LateParallelParser::reset() {
  chart.clear();
}

void LateParallelParser::parse() {
  // Insert rules of the form (START -> . a, 0) into C[0].
  vector<State> init;
  for (const rule &r : grammar[Grammar::START_SYMBOL]) {
    State s (&r, 0);
    chart.insert(s);
    init.push_back(s);
  }

  tbb::parallel_do(init.begin(), init.end(), LoopBody(grammar, sentence, chart));
}

void LateParallelParser::print_chart(std::ostream &strm) {
  for (const State &s : chart) {
    strm << "(0, ";
    s.print(strm, grammar);
    strm << ")" << endl;
  }
}

bool LateParallelParser::is_parallel() {
  return true;
}

int LateParallelParser::chart_size() {
  return chart.size();
}
