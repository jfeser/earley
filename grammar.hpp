#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

#include <cassert>

typedef int symbol;
typedef std::vector<symbol> rule;

class Grammar {
private:
  int symbol_count;
  int nonterminal_count;
  int terminal_count;
  std::unordered_map<std::string, symbol> symbol_codes;
  std::vector<std::string> code_symbols;
  std::vector<std::vector<std::vector<symbol> > > rules;

  void init(std::istream& is, std::string start);

public:
  Grammar(std::string fname, std::string start="START");
  Grammar(std::istream& is, std::string start="START");

  const static symbol START_SYMBOL = 0;

  symbol token(std::string symbol) const;
  std::vector<symbol> tokenize(std::string sentence) const;
  std::vector<symbol> tokenize(std::vector<std::string> sentence) const;

  std::string symbol_name(symbol symbol) const;
  std::string rule_name(int rule) const;

  // Returns the rules starting with a nonterminal.
  const std::vector<rule>& operator [](symbol nonterminal) const {
    return rules[nonterminal];
  }

  inline bool is_nonterminal(symbol symbol) const {
    return symbol < nonterminal_count;
  }

  inline bool is_terminal(symbol symbol) const {
    return !is_nonterminal(symbol);
  }

  static inline symbol lhs(const rule &rule) {
    return rule[0];
  }

  static inline unsigned long rhs_size(const rule &rule) {
    return rule.size() - 1;
  }

  static inline symbol rhs(const rule &rule, int idx) {
    assert(idx + 1 < rule.size());
    return rule[idx + 1];
  }
};

#endif // GRAMMAR_H
