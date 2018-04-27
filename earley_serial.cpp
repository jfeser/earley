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

#include "state.hpp"
#include "grammar.hpp"

typedef vector<unordered_set<State> > chart;
typedef deque<State> worklist;

inline void insert(int k, State new_state, chart &chart, vector<worklist> &worklist) {
  bool did_insert = chart[k].insert(new_state).second;
  if (did_insert) {
    worklist[k].push_back(new_state);
  }
}

bool parse(const Grammar &grammar, const vector<int> &words) {
  chart chart (words.size());
  vector<worklist> worklist(words.size());

  // Insert rules of the form (START -> . a, 0) into C[0].
  for (rule r : grammar[Grammar::START_SYMBOL]) {
    insert(0, State(r), chart, worklist);
  }

  for (int k = 0; k < words.size(); k++) {
    while (worklist[k].size() > 0) {
      State state = *worklist[k].begin();
      worklist[k].pop_front();

      if (!state.is_finished()) {
        symbol next_elem = state.next_symbol();
        if (grammar.is_nonterminal(next_elem)) {
          for (rule r : grammar[next_elem]) {
            insert(k, State(r, k), chart, worklist);
          }
        } else {
          if (k + 1 < chart.size()) {
            if (words[k] == next_elem) {
              insert(k+1, state.incr_pos(), chart, worklist);
            }
          }
        }
      } else {
        int &origin = state.origin;
        for (auto s = chart[origin].begin(); s != chart[origin].end(); ++s) {
          if (!s->is_finished() && s->next_symbol() == state.lhs()) {
            insert(k, s->incr_pos(), chart, worklist);
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

  Grammar g (grammar_f);

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
