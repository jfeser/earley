#ifndef LATE_PARALLEL_H
#define LATE_PARALLEL_H

#include <vector>
#include <string>

#include "tbb/concurrent_unordered_set.h"

#include "state.hpp"
#include "grammar.hpp"
#include "iparser.hpp"

class LateParallelParser : public IParser {
private:
  const Grammar &grammar;
  const std::vector<int> &sentence;
  tbb::concurrent_unordered_set<State> chart;

public:
  LateParallelParser(const Grammar &g, const std::vector<int> &s) : grammar(g), sentence(s) {}

  bool is_parallel() override;
  std::string name() override;
  void reset() override;
  void parse() override;
  void print_chart(std::ostream &strm) override;
};

#endif // LATE_PARALLEL_H
