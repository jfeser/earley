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

#include "grammar.hpp"

// struct full_state {
//   state state;
//   int origin;

//   full_state(::state s, int o) : state(s), origin(o) {}
//   full_state(grammar g, rule r, int o=0) : state(g.rule_state_codes[r]), origin(o) {}

//   full_state incr_pos(grammar g) const {
//     return full_state(g.next_state(state), origin);
//   }

//   bool operator==(const full_state &other) const {
//     return (state == other.state
//             && origin == other.origin);

// };

// template <> struct hash<full_state> {
//   size_t operator()(const full_state& state) const {
//     // Compute individual hash values for first, second and third
//     // http://stackoverflow.com/a/1646913/126995
//     size_t res = 17;
//     res = res * 31 + hash<int>()(state.state);
//     res = res * 31 + hash<int>()(state.origin);
//     return res;
//   }
// };

typedef pair<int, int> full_state;
typedef vector<unordered_set<full_state> > chart;
typedef deque<full_state> worklist;

inline void insert(int k, full_state new_state, chart &chart, vector<worklist> &worklist) {
  bool did_insert = chart[k].insert(new_state).second;
  if (did_insert) {
    worklist[k].push_back(new_state);
  }
}

bool parse(struct grammar &grammar, const vector<int> &words) {
  chart chart (words.size());
  vector<worklist> worklist(words.size());

  // Insert rules of the form (START -> . a, 0) into C[0].
  for (rule rule : grammar.rules_by_nonterminal(grammar.start_symbol())) {
    insert(0, full_state(grammar.rule_state_codes[rule], rule), chart, worklist);
  }

  for (int k = 0; k < words.size(); k++) {
    while (worklist[k].size() > 0) {
      full_state state = *worklist[k].begin();
      worklist[k].pop_front();

      if (!grammar.is_finished(state.first)) {
        symbol next_elem = grammar.state_symbol(state.first);
        if (grammar.is_nonterminal(next_elem)) {
          for (rule rule : grammar.rules_by_nonterminal(next_elem)) {
            insert(k, full_state(grammar.rule_state_codes[rule], k), chart, worklist);
          }
        } else {
          if (k + 1 < chart.size()) {
            if (words[k] == next_elem) {
              insert(k+1, full_state(grammar.next_state(state.first), state.second), chart, worklist);
            }
          }
        }
      } else {
        int origin = state.second;
        for (auto s = chart[origin].begin(); s != chart[origin].end(); ++s) {
          if (grammar.code_rules[grammar.state_rule(s->first)][grammar.state_pos(s->first)] == grammar.state_symbol(s->first)) {
            insert(k, full_state(grammar.next_state(s->first), s->second), chart, worklist);
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

  struct grammar g (grammar_f);

  words_f.seekg(0, std::ios::end);
  size_t size = words_f.tellg();
  std::string buffer(size, ' ');
  words_f.seekg(0);
  words_f.read(&buffer[0], size);
  vector<int> words = g.tokenize(buffer);

  cerr << "Debug: Beginning parse." << endl;
  parse(g, words);
  cerr << "Debug: Finished parse." << endl;

  return 0;
}
