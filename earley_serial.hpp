#ifndef EARLEY_SERIAL_H
#define EARLEY_SERIAL_H

#include <deque>
#include <vector>
#include <string>
#include <unordered_set>

#include "state.hpp"
#include "grammar.hpp"
#include "iparser.hpp"

class EarleySerialParser : public IParser {
private:
  const Grammar &grammar;
  const std::vector<int> &sentence;
  std::vector<std::unordered_set<State> > chart;
  std::deque<State> worklist;

public:
  EarleySerialParser(const Grammar &g, const std::vector<int> &s) : grammar(g), sentence(s) {}

  bool is_parallel() override;
  std::string name() override;
  void reset() override;
  void parse() override;
  void print_chart() override;
};

#endif // EARLEY_SERIAL_H
