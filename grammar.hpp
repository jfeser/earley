#include <iostream>

using namespace std;

struct grammar {
  int symbol_count;
  int nonterminal_count;
  int terminal_count;
  unordered_map<string, int> symbol_codes;
  vector<string> code_symbols;

  int rule_count;
  map<vector<int>, int> rule_codes;
  vector<vector<int> > code_rules;
  vector<vector<int> > symbol_rule_codes;

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

    int token(string symbol);
    vector<int> tokenize(string sentence);
    vector<int> tokenize(vector<string> sentence);

    string symbol_name(int symbol);
    string rule_name(int rule);
    string state_name(int state);

    int start_symbol();
    int state_symbol(int state);
    int state_rule(int state);
    int state_pos(int state);
    int next_state(int state);
    bool is_finished(int state);
    bool is_start(int symbol);
    bool is_terminal(int symbol);
    bool is_nonterminal(int symbol);
};
