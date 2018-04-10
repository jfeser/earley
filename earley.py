#!/usr/bin/env python3

from pprint import pprint
import sys

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


class StateSet(object):
    def __init__(self, states=[]):
        self._states = set(states)
        self._work = list(self._states)

    def __str__(self):
        return '{%s}' % ', '.join([str(s) for s in self._states])

    __repr__ = __str__

    def add(self, s):
        if s not in self._states:
            self._states.add(s)
            self._work.append(s)

    def has_next(self):
        return self._work != []

    def next(self):
        return self._work.pop(0)


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


def scan(state, grammar):
    out = set([])
    lhs = state.next_elem()
    for rhs in grammar[lhs]:
        out.add(State(lhs, rhs))
    return out

def parse(grammar, words):
    # Create chart.
    chart = [StateSet() for _ in range(len(words))]

    def predictor(state, k):
        lhs = state.next_elem()
        for rhs in grammar[lhs]:
            chart[k].add(State(lhs, rhs, origin=k))

    def scanner(state, k):
        if k + 1 >= len(chart):
            return
        if words[k] == state.next_elem():
            chart[k+1].add(state.incr_pos())

    def completer(state, k):
        assert state.origin != k
        for s in chart[state.origin]._states:
            if s.rhs[s.pos] == state.lhs:
                chart[k].add(s.incr_pos())

    # Initialize.
    for rhs in grammar['START']:
        chart[0].add(State('START', rhs))

    for k in range(len(words)):
        while chart[k].has_next():
            state = chart[k].next()
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
    print(words)
    pprint(chart)

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
