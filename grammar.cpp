#include <fstream>
#include <sstream>
#include <algorithm>

#include <deque>
#include <string>
#include <regex>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <vector>

#include "grammar.hpp"

using namespace std;

void Grammar::init(istream& is, string start) {
  regex rule_pattern("^\\s*([^\\s#]+)\\s+([^\\s#][^#]*)\\s*(?:#.*)?$");
  regex symbol_pattern("[^\\s]+");
  regex empty_pattern("^\\s*(?:#.*)?$");
  smatch line_match;
  set< vector<string> > string_rules;
  set<string> symbols;

  //parse the rules
  int line_count = 0;
  string line;
  while (getline(is, line)) {
    line_count++;
    if(regex_match(line, line_match, rule_pattern)){
      string lhs = line_match[1];
      string rhs = line_match[2];
      symbols.insert(lhs);
      vector<string> rule = {lhs};
      smatch symbol_match;
      while(regex_search(rhs, symbol_match, symbol_pattern)){
        string symbol = symbol_match.str();
        symbols.insert(symbol);
        rule.push_back(symbol);
        rhs = symbol_match.suffix().str();
      }
      string_rules.insert(rule);
    } else if(!regex_match(line, line_match, empty_pattern)){
      cerr << "error: invalid grammar syntax on line: " << line_count << endl;
      exit(1);
    }
  }

  //separate nonterminals from terminals
  unordered_set<string> nonterminals;
  for(auto r = string_rules.begin(); r != string_rules.end(); r++){
    nonterminals.insert((*r)[0]);
  }
  nonterminal_count = nonterminals.size();

  //create symbol codes
  symbol_codes = unordered_map<string, int>();
  code_symbols = vector<string>();

  //symbol codes start with START
  if(nonterminals.find(start) == nonterminals.end()){
    cerr << "error: start nonterminal " << start << " not found" << endl;
    exit(1);
  }

  symbol_codes[start] = 0;
  code_symbols.push_back(start);
  symbol_count = 1;

  //symbol codes continue with nonterminals
  for(auto sym = nonterminals.begin(); sym != nonterminals.end(); sym++){
    if(symbol_codes.find(*sym) == symbol_codes.end()){
      symbol_codes[*sym] = symbol_count;
      code_symbols.push_back(*sym);
      symbol_count++;
    }
  }

  //symbol codes finish with terminals
  for(auto sym = symbols.begin(); sym != symbols.end(); sym++){
    if(symbol_codes.find(*sym) == symbol_codes.end()){
      symbol_codes[*sym] = symbol_count;
      code_symbols.push_back(*sym);
      symbol_count++;
    }
  }

  //finish up the counts of symbols
  terminal_count = symbol_count - nonterminal_count;

  //create rule codes
  rules = vector<vector<vector<int> > >(nonterminal_count);
  for (int i = 0; i < nonterminal_count; i++) {
    rules[i] = vector<vector<int> >();
  }
  for(auto r = string_rules.begin(); r != string_rules.end(); r++){
    vector<int> compressed_rule;
    for(auto sym = r->begin(); sym != r->end(); sym++){
      compressed_rule.push_back(symbol_codes[*sym]);
    }
    symbol lhs = compressed_rule[0];
    rules[lhs].push_back(compressed_rule);
  }
}

Grammar::Grammar(istream& is, string start) {
  Grammar::init(is, start);
}

Grammar::Grammar(string fname, string start) {
  ifstream myfile;
  myfile.open(fname);
  Grammar::init(myfile, start);
  myfile.close();
}

symbol Grammar::token(string symbol) const {
  if(symbol_codes.find(symbol) == symbol_codes.end()){
    cerr << "error: unrecognized symbol " << symbol << endl;
    exit(1);
  }
  int code = symbol_codes.at(symbol);
  if(is_nonterminal(code)){
    cerr << "error: cannot parse nonterminal symbol " << symbol << endl;
    exit(1);
  }
  return code;
}

vector<int> Grammar::tokenize(string sentence) const {
  vector<int> tokens;
  regex symbol_pattern("[^\\s]+");
  smatch symbol_match;
  while(regex_search(sentence, symbol_match, symbol_pattern)){
    tokens.push_back(token(symbol_match.str()));
    sentence = symbol_match.suffix().str();
  }
  return tokens;
}

vector<int> Grammar::tokenize(vector<string> sentence) const {
  vector<int> tokens;
  for(auto sym = sentence.begin(); sym != sentence.end(); sym++){
    tokens.push_back(token(*sym));
  }
  return tokens;
}

string Grammar::symbol_name(int symbol) const {
  return code_symbols[symbol];
}
