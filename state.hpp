#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <vector>

#include "grammar.hpp"

class State {
public:
  const rule *rule;

  // The position of the dot in the rule.
  unsigned long pos;

  // The Earley set where the rule was first introduced.
  int origin;

  // The Earley set containing this state.
  int loc;

  State(const ::rule *rule, int loc, int origin=0, unsigned long pos=0) :
    rule(rule), pos(pos), origin(origin), loc(loc) {
    assert(pos <= Grammar::rhs_size(*rule));
    assert(origin >= 0);
    assert(loc >= origin);
  }

  // Copy constructor
  State(const State &state) :
    rule(state.rule), pos(state.pos), origin(state.origin), loc(state.loc) {}

  bool operator==(const State &other) const {
    return (rule == other.rule
            && pos == other.pos
            && origin == other.origin
            && loc == other.loc);
  }

  // Return the LHS of the state's rule.
  inline symbol lhs() const {
    return Grammar::lhs(*rule);
  }

  // Returns true if the state is positioned at the end of the rule.
  inline bool is_finished() const {
    return pos == Grammar::rhs_size(*rule);
  }

  // Returns the symbol after the current position in the rule.
  inline symbol next_symbol() const {
    assert(!is_finished());
    return Grammar::rhs(*rule, pos);
  }

  // Create a new state with an incremented position.
  inline State incr_pos(int new_loc) const {
    return State(rule, new_loc, origin, pos + 1);
  }

  std::ostream& print(std::ostream& os, const Grammar &grammar) const {
    os << "(" << loc << ", " << grammar.symbol_name(lhs()) << " -> ";
    for (auto i = 0ul; i < Grammar::rhs_size(*rule); i++) {
      if (pos == i) { os << "• "; }
      os << grammar.symbol_name(Grammar::rhs(*rule, i));
      if (i < Grammar::rhs_size(*rule) - 1) { os << " "; }
    }
    if (pos == Grammar::rhs_size(*rule)) { os << " •"; }
    os << ", " << origin << ")";
    return os;
  }
};

template <> struct std::hash<State> {
  size_t operator()(const State& state) const {
    // Compute individual hash values for first, second and third
    // http://stackoverflow.com/a/1646913/126995
    size_t res = 17;
    res = res * 31 + hash<void * >()((void *)(state.rule));
    res = res * 31 + hash<int>()(state.pos);
    res = res * 31 + hash<int>()(state.origin);
    return res;
  }
};

#endif // STATE_H
