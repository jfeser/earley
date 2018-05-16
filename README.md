# LATE AinT Earley: A Faster Parallel Earley Parser

> We present the LATE algorithm, an asynchronous variant of the Earley algorithm for parsing context free grammars. The Earley algorithm is naturally task based, but is difficult to parallelize because of dependencies between the tasks. We present the LATE algorithm, which uses additional datastructures to maintain information about the state of the parse so that work items may be processed in any order. This property allows the LATE algorithm to be sped up using task parallelism. We show that the LATE algorithm can achieve a 120x speedup over the Earley algorithm on a natural language task.

Read the paper here: TODO

# Building

1. Install [Threading Building Blocks (TBB) 2018 Update 3](https://github.com/01org/tbb/releases).
2. Set `TBB_PREFIX` in the Makefile to the correct installation prefix for TBB. It defaults to `/opt/intel/tbb`.
3. Set `CXX` in the Makefile to your preferred compiler. It defaults to `clang++-5.0`.
4. Run `make`

All the relevant datasets exist in the repository, but the weak scaling benchmark inputs can be regenerated by running `gen-weak-scaling.sh`.

# Running

The primary interface to the parsing algorithms is a tool called `parse`. It parses the input and outputs timing information in CSV format.

```
Usage: parse [-n N] [-p PARSER] GRAMMAR CORPUS

Arguments:
  -n N        The number of threads. If not provided, jobs will be run from 1 to
              # cores threads.
  -p PARSER   The parsing algorithm to use. Must be one of: earley_serial, late_serial, 
              late_parallel. If not provided, all implementations will be run.
  GRAMMAR     The grammar to parse.
  CORPUS      The input sentence. Expected to be a space separated list of tokens.
```

`ambiguate.py` is a helper program which takes a seed grammar and generates a more ambiguous grammar which parses the same language.

```
Usage: ambiguate.py GRAMMAR N

Arguments:
  GRAMMAR     The grammar to transform.
  N           The number of times to replicate each nonterminal.
```

# Running benchmarks

Run the benchmarks with `run-bench.sh`. Results will be generated in the `results/` folder. `Analysis.ipynb` can be used to generate the plots.
