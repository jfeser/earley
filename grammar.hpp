#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

using namespace std;

typedef int rule;
typedef int state;
typedef int symbol;

struct grammar {
  int symbol_count;
  int nonterminal_count;
  int terminal_count;
  unordered_map<string, symbol> symbol_codes;
  vector<string> code_symbols;

  int rule_count;
  map<vector<int>, rule> rule_codes;
  vector<vector<int> > code_rules;
  vector<vector<rule> > symbol_rule_codes;

  int state_count;
  vector<int> rule_state_codes;
  vector<int> state_rule_codes;
  vector<int> state_pos_codes;
  vector<int> state_symbol_codes;
  vector<int> state_state_codes;
  vector<vector<int> > symbol_state_codes;

private:
  void init(istream& is, string start);

public:
  grammar(string fname, string start="START");
  grammar(istream& is, string start="START");

  symbol token(string symbol);
  vector<symbol> tokenize(string sentence);
  vector<symbol> tokenize(vector<string> sentence);

  string symbol_name(symbol symbol);
  string rule_name(rule rule);
  string state_name(state state);

  // Returns the rules starting with a nonterminal.
  const vector<rule>& rules_by_nonterminal(symbol nonterminal);

  // Create a new state.
  state create_state(rule rule, int pos=0);

  symbol start_symbol();
  symbol state_symbol(state state);
  rule state_rule(state state);
  int state_pos(state state);
  state next_state(state state);
  bool is_finished(state state);
  bool is_start(symbol symbol);
  bool is_terminal(symbol symbol);
  bool is_nonterminal(symbol symbol);
};
