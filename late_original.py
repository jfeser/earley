#!/usr/bin/env python3

from pprint import pprint
from collections import defaultdict
import sys
from random import randint

def parse_grammar(grammar_file):
    grammar = {}
    # Parse grammar
    for line in grammar_file:
        line = line.strip() # Strip trailing whitespace
        line = line.split('#')[0] # Stop reading after comment starts
        if len(line) == 0: continue

        # Split into component parts
        line = line.split()
        lhs = line[0]
        rhs = line[1:]

        # Add to grammar
        if lhs not in grammar:
            grammar[lhs] = []
        grammar[lhs].append(rhs)
    return grammar

class State(object):
    def __init__(self, lhs, rhs, pos=0, origin=0):
        self.lhs = lhs
        self.rhs = tuple(rhs)
        self.pos = pos
        self.origin = origin

    def __str__(self):
        rhs_dot = list(self.rhs)
        rhs_dot.insert(self.pos, '•')
        rhs_str = ' '.join(rhs_dot)
        return '(%s -> %s, %d)' % (self.lhs, rhs_str, self.origin)

    __repr__ = __str__

    def __eq__(self, other):
        return (self.lhs, self.rhs, self.pos, self.origin) == \
            (other.lhs, other.rhs, other.pos, other.origin)

    def __hash__(self):
        return hash((self.lhs, self.rhs, self.pos, self.origin))

    def finished(self):
        return self.pos == len(self.rhs)

    def next_elem(self):
        return self.rhs[self.pos]

    def incr_pos(self):
        return State(self.lhs, self.rhs, self.pos + 1, self.origin)

class StateSet(object):
    def __init__(self, work, states=[]):
        self._states = set(states)
        self._work = work

    def __str__(self):
        return '{%s}' % ', '.join([str(s) for s in self._states])

    __repr__ = __str__

    def add(self, s):
        if s not in self._states:
            self._states.add(s)
            self._work.append(s)

def parse(grammar, words):
    # Create chart.
    work = []
    chart = [set() for _ in range(len(words))]
    requested = [dict() for _ in range(len(words))]
    completed = [dict() for _ in range(len(words))]

    def queue(state, k):
      if state not in chart[k]:
        work.append((state, k))
      chart[k].add(state)

    def predictor(state, k):
        lhs = state.next_elem()
        if lhs in requested[k]:
            requested[k][lhs].append(state)
        else:
            requested[k][lhs] = [state]
            for rhs in grammar[lhs]:
                queue(State(lhs, rhs, origin=k), k)
        if lhs in completed[k]:
            for dest in completed[k][lhs]:
                queue(state.incr_pos(), dest)

    def scanner(state, k):
        if k + 1 >= len(chart):
            return
        if words[k] == state.next_elem():
            queue(state.incr_pos(), k + 1)

    def completer(state, k):
        assert state.origin != k
        if state.lhs in completed[state.origin]: # TODO completed should just be a set of symbol destination pairs where you check if someone in your set has already completed this symbol in this location before. Then we have a separate dict between symbols and destination lists
            if k in completed[state.origin][state.lhs]:
                return # This is a little overdramatic. It would be okay to continue, it would just repeat work. It might be interesting to instead store a list of completed rules instead of a set of destination locations, but that would of course mate the predictor step harder.
            else:
                completed[state.origin][state.lhs].add(k)
        else:
            completed[state.origin][state.lhs] = set([k])
        for requester in requested[state.origin][state.lhs]:
            queue(requester.incr_pos(), k)

    # Initialize. (more complicated than I thought)
    predictor(State('START', ['START']), 0)

    while work != []:
        (state, k) = work.pop(randint(0, len(work) - 1))
        if not state.finished():
            if state.next_elem() in grammar:
                predictor(state, k)
            else:
                scanner(state, k)
        else:
            completer(state, k)
    return chart

def main(grammar_file, in_file):
    grammar = parse_grammar(grammar_file)
    words = in_file.read().split()
    chart = parse(grammar, words)
    for (k, states) in enumerate(chart):
      for state in states:
        print("SET: {} STATE: {}".format(k, state))

if __name__ == '__main__':
    if len(sys.argv) > 3 or len(sys.argv) < 2:
        print('Usage: earley.py GRAMMAR [FILE]')
        exit(1)
    grammar_file = open(sys.argv[1], 'r')
    if len(sys.argv) == 3:
        in_file = open(sys.argv[2], 'r')
    else:
        in_file = sys.stdin
    main(grammar_file, in_file)
