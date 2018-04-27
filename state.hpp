#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <vector>

#include "grammar.hpp"

using namespace std;

class State {
public:
  rule &rule;
  unsigned long pos;
  int origin;

  State(::rule &rule, unsigned long pos=0, int origin=0) :
    rule(rule), pos(pos), origin(origin) {}

  // Copy constructor
  State(const State &state) :
    rule(state.rule), pos(state.pos), origin(state.origin) {}

  bool operator==(const State &other) const {
    return (rule == other.rule
            && pos == other.pos
            && origin == other.origin);
  }

  // Return the LHS of the state's rule.
  inline symbol lhs() const {
    return Grammar::lhs(rule);
  }

  // Returns true if the state is positioned at the end of the rule.
  inline bool is_finished() const {
    return pos == Grammar::rhs_size(rule);
  }

  // Returns the symbol after the current position in the rule.
  inline symbol next_symbol() const {
    return Grammar::rhs(rule, pos);
  }

  // Create a new state with an incremented position.
  inline State incr_pos() const {
    State new_state = *this;
    new_state.pos++;
    return new_state;
  }

  ostream& print(ostream& os, const Grammar &grammar) const {
    os << "(" << grammar.symbol_name(lhs()) << " -> ";
    for (auto i = 0ul; i < Grammar::rhs_size(rule); i++) {
      if (pos == i) { os << "• "; }
      os << grammar.symbol_name(Grammar::rhs(rule, i));
      if (i < Grammar::rhs_size(rule) - 1) { os << " "; }
    }
    os << ", " << origin << ")";
    return os;
  }
};

template <> struct hash<State> {
  size_t operator()(const State& state) const {
    // Compute individual hash values for first, second and third
    // http://stackoverflow.com/a/1646913/126995
    size_t res = 17;
    res = res * 31 + hash<void * >()((void *)(&state.rule));
    res = res * 31 + hash<int>()(state.pos);
    res = res * 31 + hash<int>()(state.origin);
    return res;
  }
};

#endif // STATE_H
