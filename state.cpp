#include "state.hpp"

size_t tbb_hasher(const State& x) {
  return x.hash();
}
