#include <iostream>
#include <sstream>
#include <fstream>

#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"

#include "iparser.hpp"
#include "grammar.hpp"
#include "late_parallel.hpp"
#include "late_serial.hpp"
#include "earley_serial.hpp"
#include "earley_parallel.hpp"

using namespace std;

const int max_repeats = 100;
const double max_time = 30.0;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Usage: parse GRAMMAR FILE" << endl;
    return 1;
  }

  cerr << "Loading grammar..." << endl;
  ifstream grammar_f (argv[1]);
  ifstream words_f (argv[2]);

  Grammar g (grammar_f);

  cerr << "Lexing input string..." << endl;
  words_f.seekg(0, std::ios::end);
  size_t size = words_f.tellg();
  std::string buffer(size, ' ');
  words_f.seekg(0);
  words_f.read(&buffer[0], size);
  vector<int> words = g.tokenize(buffer);

  cerr << "Parsing..." << endl;
  vector<IParser*> parsers = {
    new LateParallelParser(g, words),
    // new LateSerialParser(g, words),
    // new EarleySerialParser(g, words),
    // new EarleyParallelParser(g, words),
  };

  cout << "name,time,threads" << endl;
  for (IParser *parser : parsers) {
    int n = parser->is_parallel() ? tbb::task_scheduler_init::default_num_threads() : 1;
    for (int p = 1; p <= n; ++p) {
      // Construct task scheduler with p threads
      tbb::task_scheduler_init init(p);

      double min = 1.0/0.0, tot = 0.0;
      int r = 0;
      while (r < max_repeats && tot < max_time) {
        parser->reset();
        tbb::tick_count t0 = tbb::tick_count::now();
        parser->parse();
        tbb::tick_count t1 = tbb::tick_count::now();
        double t = (t1 - t0).seconds();

        if (t < min) { min = t; }
        tot += t;
        r++;
      }
      cout << parser->name() << "," << min << "," << tot << "," << r << "," << p << endl;

      ostringstream out_fn;
      out_fn << parser->name() << "-" << p << ".txt";
      ofstream out_f (out_fn.str());
      parser->print_chart(out_f);
      out_f.flush();
    }
  }

  return 0;
}
