#include "late_util.hpp"

size_t tbb_hasher(const struct word& x) {
  return x.hash();
}

size_t tbb_hasher(const struct msg& x) {
  return x.hash();
}
