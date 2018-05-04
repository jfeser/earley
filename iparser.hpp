#ifndef IPARSER_H
#define IPARSER_H

#include <string>

class IParser {
public:
  virtual std::string name() { return ""; };
  virtual bool is_parallel() { return false; };
  virtual void reset() {};
  virtual void parse() {};
  virtual void print_chart() {};
};

#endif // IPARSER_H
