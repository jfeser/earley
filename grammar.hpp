#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

using namespace std;

typedef int symbol;
typedef vector<symbol> rule;

class Grammar {
private:
  int symbol_count;
  int nonterminal_count;
  int terminal_count;
  unordered_map<string, symbol> symbol_codes;
  vector<string> code_symbols;
  vector<vector<vector<symbol> > > rules;

  void init(istream& is, string start);

public:
  Grammar(string fname, string start="START");
  Grammar(istream& is, string start="START");

  const static symbol START_SYMBOL = 0;

  symbol token(string symbol) const;
  vector<symbol> tokenize(string sentence) const;
  vector<symbol> tokenize(vector<string> sentence) const;

  string symbol_name(symbol symbol) const;
  string rule_name(int rule) const;

  // Returns the rules starting with a nonterminal.
  const vector<rule>& operator [](symbol nonterminal) const {
    return rules[nonterminal];
  }

  inline bool is_nonterminal(symbol symbol) const {
    return symbol < nonterminal_count;
  }

  inline bool is_terminal(symbol symbol) const {
    return !is_nonterminal(symbol);
  }

  static inline symbol lhs(rule rule) {
    return rule[0];
  }

  static inline unsigned long rhs_size(rule rule) {
    return rule.size() - 1;
  }

  static inline symbol rhs(rule rule, int idx) {
    return rule[idx + 1];
  }
};

#endif // GRAMMAR_H
