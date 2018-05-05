#ifndef IPARSER_H
#define IPARSER_H

#include <ostream>
#include <string>

class IParser {
public:
  virtual std::string name() = 0;
  virtual bool is_parallel() = 0;
  virtual void reset() = 0;
  virtual void parse() = 0;
  virtual void print_chart(std::ostream &s) = 0;
};

#endif // IPARSER_H
