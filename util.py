#!/usr/bin/env python3

from collections import defaultdict
import argparse
import re
import random

def multidict():
  return defaultdict(list)

def main(parse):
    # Parse arguments.
    parser = argparse.ArgumentParser()
    parser.add_argument("grammar", help="path to grammar file")
    parser.add_argument("corpus", help="path to corpus file", default = "")
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

    if args.corpus == "":
        corpus = sys.stdin
    else:
        corpus = open(args.corpus, 'r')

    num = 0
    for line in corpus:
        sentence = line.split("#")[0].split()
        if sentence:
            chart = parse(grammar, sentence)
            for item in chart:
                print("(%d, %s)" % (num, item))
            num += 1

    if args.corpus != "":
        corpus.close()

class State(object):
    def __init__(self, lhs, rhs, loc, origin=None, pos=0):
        assert len(rhs) >= pos
        self.lhs = lhs
        self.rhs = tuple(rhs)
        self.pos = pos
        self.loc = loc
        if origin == None:
            self.origin = self.loc
        else:
            self.origin = origin

    def __str__(self):
        rhs_dot = list(self.rhs)
        rhs_dot.insert(self.pos, '•')
        rhs_str = ' '.join(rhs_dot)
        return '(%d, %s -> %s, %d)' % (self.loc, self.lhs, rhs_str, self.origin)

    __repr__ = __str__

    def __eq__(self, other):
        return (self.loc, self.origin, self.lhs, self.rhs, self.pos) == \
            (other.loc, other.origin, other.lhs, other.rhs, other.pos)

    def __lt__(self, other):
        return (self.loc, self.origin, self.lhs, self.rhs, self.pos) < \
            (other.loc, other.origin, other.lhs, other.rhs, other.pos)

    def __hash__(self):
        return hash((self.lhs, self.rhs, self.pos, self.origin, self.loc))

    def finished(self):
        return self.pos == len(self.rhs)

    def next_word(self):
        return self.rhs[self.pos]

    def incr_word(self, loc):
        return State(self.lhs, self.rhs, loc, self.origin, self.pos + 1)

