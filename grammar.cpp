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

void grammar::init(istream& is, string start) {
  regex rule_pattern("^\\s*([^\\s#]+)\\s+([^\\s#][^#]*)\\s*(?:#.*)?$");
  regex symbol_pattern("[^\\s]+");
  regex empty_pattern("^\\s*(?:#.*)?$");
  smatch line_match;
  set< vector<string> > rules;
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
      rules.insert(rule);
    } else if(!regex_match(line, line_match, empty_pattern)){
      cerr << "error: invalid grammar syntax on line: " << line_count << endl;
      exit(1);
    }
  }

  //separate nonterminals from terminals

  unordered_set<string> nonterminals;
  for(auto rule = rules.begin(); rule != rules.end(); rule++){
    nonterminals.insert((*rule)[0]);
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

  for(auto symbol = nonterminals.begin(); symbol != nonterminals.end(); symbol++){
    if(symbol_codes.find(*symbol) == symbol_codes.end()){
      symbol_codes[*symbol] = symbol_count;
      code_symbols.push_back(*symbol);
      symbol_count++;
    }
  }

  //symbol codes finish with terminals

  for(auto symbol = symbols.begin(); symbol != symbols.end(); symbol++){
    if(symbol_codes.find(*symbol) == symbol_codes.end()){
      symbol_codes[*symbol] = symbol_count;
      code_symbols.push_back(*symbol);
      symbol_count++;
    }
  }

  //finish up the counts of symbols

  terminal_count = symbol_count - nonterminal_count;

  //create rule codes

  rule_codes = map<vector<int>, int>();
  code_rules = vector<vector<int> >();
  symbol_rule_codes = vector<vector<int> >(nonterminal_count, vector<int>());
  rule_count = 0;

  for(auto rule = rules.begin(); rule != rules.end(); rule++){
    vector<int> compressed_rule;
    for(auto symbol = rule->begin(); symbol != rule->end(); symbol++){
      compressed_rule.push_back(symbol_codes[*symbol]);
    }
    rule_codes[compressed_rule] = rule_count;
    code_rules.push_back(compressed_rule);
    symbol_rule_codes[compressed_rule[0]].push_back(rule_count);
    rule_count += 1;
  }

  //create state codes

  symbol_state_codes = vector<vector<int> >(nonterminal_count, vector<int>());
  state_rule_codes = vector<int>();
  state_pos_codes = vector<int>();
  state_symbol_codes = vector<int>();
  state_state_codes = vector<int>();
  rule_state_codes = vector<int>();

  state_count = 0;

  for(int rule = 0; rule < rule_count; rule++){
    symbol_state_codes[code_rules[rule][0]].push_back(state_count);
    rule_state_codes.push_back(state_count);
    int pos = 0;
    for(auto symbol = code_rules[rule].begin() + 1; symbol != code_rules[rule].end(); symbol++){
      state_rule_codes.push_back(rule);
      state_pos_codes.push_back(pos);
      state_symbol_codes.push_back(*symbol);
      pos++;
      state_count++;
      state_state_codes.push_back(state_count);
    }
    state_rule_codes.push_back(rule);
    state_pos_codes.push_back(pos);
    state_symbol_codes.push_back(code_rules[rule][0]);
    state_count++;
    state_state_codes.push_back(-1);
  }
}

grammar::grammar(istream& is, string start) {
  grammar::init(is, start);
}

grammar::grammar(string fname, string start) {
  ifstream myfile;
  myfile.open(fname);
  grammar::init(myfile, start);
  myfile.close();
}

int grammar::token(string symbol) {
  if(symbol_codes.find(symbol) == symbol_codes.end()){
    cerr << "error: unrecognized symbol " << symbol << endl;
    exit(1);
  }
  int code = symbol_codes[symbol];
  if(is_nonterminal(code)){
    cerr << "error: cannot parse nonterminal symbol " << symbol << endl;
    exit(1);
  }
  return code;
}

vector<int> grammar::tokenize(string sentence) {
  vector<int> tokens;
  regex symbol_pattern("[^\\s]+");
  smatch symbol_match;
  while(regex_search(sentence, symbol_match, symbol_pattern)){
    tokens.push_back(token(symbol_match.str()));
    sentence = symbol_match.suffix().str();
  }
  return tokens;
}

vector<int> grammar::tokenize(vector<string> sentence) {
  vector<int> tokens;
  for(auto symbol = sentence.begin(); symbol != sentence.end(); symbol++){
    tokens.push_back(token(*symbol));
  }
  return tokens;
}

string grammar::symbol_name(int symbol){
  return code_symbols[symbol];
}

string grammar::rule_name(int rule){
  stringstream result;
  result << symbol_name(code_rules[rule][0]) << " ->";
  for(auto symbol = code_rules[rule].begin() + 1; symbol != code_rules[rule].end(); symbol++){
    result << " " << symbol_name(*symbol);
  }
  return result.str();
}

string grammar::state_name(int state){
  stringstream result;
  int rule = state_rule(state);
  result << symbol_name(code_rules[rule][0]) << " ->";
  int pos = 0;
  for(auto symbol = code_rules[rule].begin() + 1; symbol != code_rules[rule].end(); symbol++){
    if(pos == state_pos(state)){
      result << " •";
    }
    result << " " << symbol_name(*symbol);
    pos++;
  }
  if(pos == state_pos(state)){
    result << " <>";
  }
  return result.str();
}

int grammar::state_symbol(int state){
  return state_symbol_codes[state];
}

int grammar::state_rule(int state){
  return state_rule_codes[state];
}

int grammar::state_pos(int state){
  return state_pos_codes[state];
}

int grammar::next_state(int state){
  return state_state_codes[state];
}

bool grammar::is_finished(int state){
  return next_state(state) == -1;
}

bool grammar::is_start(int symbol){
  return symbol == 0;
}

bool grammar::is_terminal(int symbol){
  return !is_nonterminal(symbol);
}

bool grammar::is_nonterminal(int symbol){
  return symbol < nonterminal_count;
}

const vector<int>& grammar::rules_by_nonterminal(int symbol) {
  return symbol_rule_codes[symbol];
}

int start_symbol(){
  return 0;
}

int main(int argc, char** argv){
  grammar g(argv[1]);
  auto tokens = g.tokenize("1 + ( 2 + 3 )");
  for(auto token = tokens.begin(); token != tokens.end(); token++){
    std::cout << *token << endl;
  }
  for(auto token = g.code_symbols.begin(); token != g.code_symbols.end(); token++){
    std::cout << *token << endl;
  }
}
