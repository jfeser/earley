#ifndef LATE_UTIL
#define LATE_UTIL

#include "grammar.hpp"

struct word {
  symbol lhs;
  int origin, loc;

  word(symbol l, int o, int c) : lhs(l), origin(o), loc(c) {}

  inline bool operator==(const struct word &other) const {
    return (lhs == other.lhs
            && origin == other.origin
            && loc == other.loc);
  }

  inline size_t hash() const {
    size_t res = 17;
    res = res * 31 + std::hash<symbol>()(lhs);
    res = res * 31 + std::hash<int>()(origin);
    res = res * 31 + std::hash<int>()(loc);
    return res;
  }
};

struct msg {
  symbol lhs;
  int origin;

  msg(symbol l, int o) : lhs(l), origin(o) {}

  inline bool operator==(const struct msg &other) const {
    return (lhs == other.lhs && origin == other.origin);
  }

  inline size_t hash() const {
    size_t res = 17;
    res = res * 31 + std::hash<symbol>()(lhs);
    res = res * 31 + std::hash<int>()(origin);
    return res;
  }
};

template <> struct std::hash<struct word> {
  size_t operator()(const struct word& w) const { return w.hash(); }
};

template <> struct std::hash<struct msg> {
  size_t operator()(const struct msg& r) const { return r.hash(); }
};

size_t tbb_hasher(const struct word& x);
size_t tbb_hasher(const struct msg& x);

#endif // LATE_UTIL
