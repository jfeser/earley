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

using namespace std;

typedef multimap<string, vector<string> > grammar;

struct state {
  grammar::const_iterator rule;
  int pos, origin;

  inline const string &lhs() const {
    return this->rule->first;
  }

  inline const vector<string> &rhs() const {
    return this->rule->second;
  }

  state(const grammar::const_iterator rule) :
    rule(rule), pos(0), origin(0) {}

  state(const state &state) :
    rule(state.rule), pos(state.pos), origin(state.origin) {}

  state(grammar::iterator &rule, int pos, int origin) :
    rule(rule), pos(pos), origin(origin) {}

  bool operator==(const state &other) const {
    return (rule == other.rule
            && pos == other.pos
            && origin == other.origin);
  }
};

ostream& operator<<(ostream& os, const state& state) {
  os << "(" << state.lhs() << " -> ";
  for (int i = 0; i < state.rhs().size(); i++) {
    if (state.pos == i) {
      os << "• ";
    }
    os << state.rhs()[i];
    if (i < state.rhs().size() - 1) {
      os << " ";
    }
  }
  os << ", " << state.origin << ")";
  return os;
}

template <> struct hash<state> {
  size_t operator()(const state& state) const {
    // Compute individual hash values for first, second and third
    // http://stackoverflow.com/a/1646913/126995
    size_t res = 17;
    res = res * 31 + hash<void * >()((void *)(&*state.rule));
    res = res * 31 + hash<int>()(state.pos);
    res = res * 31 + hash<int>()(state.origin);
    return res;
  }
};

inline bool finished(state &state) {
  return state.pos == state.rhs().size();
}

inline const string& next_elem(state &state) {
  return state.rhs()[state.pos];
}

inline state incr_pos(state &state) {
  struct state new_state = state;
  new_state.pos++;
  return new_state;
}

typedef vector<unordered_set<state> > chart;
typedef deque<state> worklist;

// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
      }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
      }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

vector<string> split(istream &strm) {
  vector<string> words;
  copy(istream_iterator<string>(strm),
       istream_iterator<string>(),
       back_inserter(words));
  return words;
}

vector<string> split(string &str) {
  istringstream strm(str);
  return split(strm);
}

void dump_vector(vector<string> &v) {
  cerr << "[";
  for (string x : v) {
    cerr << "\"" << x << "\", ";
  }  
  cerr << "]" << endl;
}

grammar parse_grammar(ifstream &file) {
  grammar grammar;

  string line;
  while (getline(file, line)) {
    // Strip trailing and leading whitespace.
    trim(line);

    // Strip trailing comments.
    string::size_type hash_pos = line.find_first_of('#');
    if (hash_pos != string::npos) {
      line = line.substr(0, hash_pos);
    }

    if (line.length() == 0) { continue; }

    vector<string> rule = split(line);
    if (rule.size() == 1) {
      cerr << "Error: Rule without RHS." << endl;
      exit(1);
    }

    string lhs = rule[0];
    rule.erase(rule.begin());

    cerr << "Debug: Inserting grammar rule: " << lhs << "\t";
    for (string sym : rule) {
      cerr << sym << " ";
    }
    cerr << endl;

    grammar.insert(pair <string, vector<string> > (lhs, rule));
  }

  return grammar;
}

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
