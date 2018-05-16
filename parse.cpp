#include <iostream>
#include <sstream>
#include <fstream>

#include <getopt.h>
#include <stdlib.h>

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
const double max_time = 1.0;

int main(int argc, char *argv[]) {
  int c, n_threads = -1;
  char *c_parser_name = NULL;
  while ((c = getopt (argc, argv, "n:p:")) != -1) {
    switch (c) {
    case 'n':
      n_threads = atoi(optarg);
      break;
    case 'p':
      c_parser_name = optarg;
      break;
    case '?':
      if (optopt == 'n' || optopt == 'p')
        fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;
    default:
      abort ();
    }
  }

  ifstream grammar_f (argv[optind++]);
  ifstream words_f (argv[optind++]);

  Grammar g (grammar_f);

  words_f.seekg(0, std::ios::end);
  size_t size = words_f.tellg();
  std::string buffer(size, ' ');
  words_f.seekg(0);
  words_f.read(&buffer[0], size);
  vector<int> words = g.tokenize(buffer);

  vector<IParser*> parsers;
  if (c_parser_name == NULL) {
    parsers.push_back(new LateParallelParser(g, words));
    parsers.push_back(new LateSerialParser(g, words));
    parsers.push_back(new EarleySerialParser(g, words));
  } else {
    string parser_name (c_parser_name);
    if (parser_name == "late_parallel") {
      parsers.push_back(new LateParallelParser(g, words));
    } else if (parser_name == "late_serial") {
      parsers.push_back(new LateSerialParser(g, words));
    } else if (parser_name == "earley_serial") {
      parsers.push_back(new EarleySerialParser(g, words));
    } else {
      cerr << "Unexpected parser name: " << parser_name << endl;
      return 1;
    }
  }

  cout << "name,avg_time,tot_time,repeats,threads,chart_size" << endl;
  for (IParser *parser : parsers) {
    int min_threads, max_threads;
    if (n_threads == -1) {
      min_threads = 1;
      max_threads = parser->is_parallel() ? tbb::task_scheduler_init::default_num_threads() : 1;
    } else {
      min_threads = max_threads = n_threads;
    }

    for (int p = min_threads; p <= max_threads; ++p) {
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
      cout << parser->name()
           << "," << min
           << "," << tot
           << "," << r
           << "," << p
           << "," << parser->chart_size()
           << endl;

      ostringstream out_fn;
      out_fn << parser->name() << "-" << p << ".txt";
      ofstream out_f (out_fn.str());
      parser->print_chart(out_f);
      out_f.flush();
    }
  }

  return 0;
}
