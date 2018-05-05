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
  std::vector<std::deque<State> > worklist;

  inline void insert(int k, State new_state);

public:
  EarleySerialParser(const Grammar &g, const std::vector<int> &s) : grammar(g), sentence(s) {
    reset();
  }

  bool is_parallel() override;
  std::string name() override;
  void reset() override;
  void parse() override;
  void print_chart(std::ostream &strm) override;
};

#endif // EARLEY_SERIAL_H
