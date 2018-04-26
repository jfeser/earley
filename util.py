#!/usr/bin/env python3

from collections import defaultdict
import argparse
import re
import random

def main(parse):
    # Parse arguments.
    parser = argparse.ArgumentParser()
    parser.add_argument("grammar", help="path to grammar file")
    parser.add_argument("sentences", help="path to sentence file", default = "")
    parser.add_argument("--seed", type=int, default=0, help="RNG seed")
    args = parser.parse_args()

    # Create a random generator.
    random.seed(args.seed)

    grammar = defaultdict(list)

    with open(args.grammar, 'r') as grammar_file:
      for (num, line) in enumerate(grammar_file):
        error_endl = " on line {} of {}".format(num, args.grammar)

        match = re.match(r"^\s*([^\s#]+)\s+([^#]*?)\s*(?:#.*)?$", line)
        if match:
          lhs = match.group(1)
          rhs = match.group(2).split()
          grammar[lhs].append(rhs)
        elif re.match(r"^\s*(?:#.*)?$", line):
          pass
        else:
          raise ValueError("Invalid syntax on line: " + error_endl)

    if args.sentences == "":
        sentences = sys.stdin
    else:
        sentences = open(args.sentences, 'r')

    num = 0
    for line in sentences:
        sentence = line.split("#")[0].split()
        if sentence:
            chart = parse(grammar, sentence)
            for item in chart:
                print("(%d, %s)" % (num, item))
            num += 1

    if args.sentences != "":
        sentences.close()
