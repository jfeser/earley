#include "earley_parallel.hpp"

#include <iostream>

#include "tbb/parallel_do.h"

using namespace std;

typedef vector<tbb::concurrent_unordered_set<State> > chart_t;

struct EarleyBody {
  const Grammar &grammar;
  const vector<int> &sentence;
  const int k;
  chart_t &chart;

  EarleyBody(const Grammar &g, chart_t &c, const vector<int> &w, const int k) :
    grammar(g), sentence(w), k(k), chart(c) {}

  inline void insert(int j, State new_state, tbb::parallel_do_feeder<State>& feeder) const {
    bool did_insert = chart[j].insert(new_state).second;
    if (did_insert) {
      // Only feed the current loop.
      if (j == k) { feeder.add(new_state); }
    }
  }

  void operator()(State st, tbb::parallel_do_feeder<State>& feeder) const {
    if (!st.is_finished()) {
      if (grammar.is_nonterminal(st.next_symbol())) {
        for (const rule &r : grammar[st.next_symbol()]) {
          insert(k, State(&r, k), feeder);
        }
      } else {
        if (k + 1 < chart.size()) {
          if (sentence[k] == st.next_symbol()) {
            insert(k+1, st.incr_pos(k+1), feeder);
          }
        }
      }
    } else {
      for (auto s = chart[st.origin].begin(); s != chart[st.origin].end(); ++s) {
        if (!s->is_finished() && s->next_symbol() == st.lhs()) {
          insert(k, s->incr_pos(k), feeder);
        }
      }
    }
  }
};

void EarleyParallelParser::parse() {
  for (const rule &r : grammar[Grammar::START_SYMBOL]) {
    chart[0].insert(State(&r, 0));
  }

  for (int k = 0; k <= sentence.size(); k++) {
    tbb::parallel_do(chart[k].begin(), chart[k].end(),
                     EarleyBody(grammar, chart, sentence, k));
  }
}

string EarleyParallelParser::name() {
  return "earley_parallel";
}

bool EarleyParallelParser::is_parallel() {
  return true;
}

void EarleyParallelParser::reset() {
  chart.clear();
  for (int i = 0; i < sentence.size() + 1; i++) {
    chart.push_back(tbb::concurrent_unordered_set<State>());
  }
}

void EarleyParallelParser::print_chart(ostream &strm) {
  for (int i = 0; i < chart.size(); i++) {
    for (State &s : chart[i]) {
      strm << "(0, ";
      s.print(strm, grammar);
      strm << ")" << endl;
    }
  }
}

int EarleyParallelParser::chart_size() {
  int size = 0;
  for (int i = 0; i < chart.size(); i++) {
    size += chart[i].size();
  }
  return size;
}
