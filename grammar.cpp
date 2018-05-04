#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <deque>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <vector>

#include "grammar.hpp"

using namespace std;

vector<string> split(string &str) {
  istringstream strm(str);
  vector<string> words;
  copy(istream_iterator<string>(strm),
       istream_iterator<string>(),
       back_inserter(words));
  return words;
}

void Grammar::init(istream& is, string start) {
  set< vector<string> > string_rules;
  set<string> symbols;

  int line_count = 0;
  string line;
  while (getline(is, line)) {
    line_count++;

    // Strip trailing spaces.
    line.erase(find_if(line.rbegin(), line.rend(), [](int ch) {
          return !isspace(ch); }).base(), line.end());

    // Strip leading spaces.
    line.erase(line.begin(), find_if(line.begin(), line.end(), [](int ch) {
          return !isspace(ch); }));

    // Strip trailing comments.
    auto hash_pos = find_if(line.rbegin(), line.rend(),
                            [](int ch) { return ch == '#'; }).base();
    if (hash_pos != line.begin()) {
      line.erase(hash_pos, line.end());
    }

    if (line.length() == 0) { continue; }

    vector<string> rule = split(line);
    if (rule.size() == 1) {
      cerr << "Error: Rule without RHS." << endl;
      exit(1);
    }

    for (string sym : rule) { symbols.insert(sym); }
    string_rules.insert(rule);
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
  for (string tok : split(sentence)) {
    tokens.push_back(token(tok));
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
