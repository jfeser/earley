#!/usr/bin/env python3

import sys

def parse_grammar(fn):
    grammar = {}
    with open(fn, 'r') as grammar_file:
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


def print_grammar(grammar):
    for (lhs, rhs) in grammar.items():
        for rule in rhs:
            print('%s\t%s' % (lhs, ' '.join(rule)))


def symbols(g):
    res = set([])
    for (lhs, rhs) in g.iteritems():
        res.add(lhs)
        for syms in rhs:
            res |= set(syms)
    return res


def nonterminals(g):
    return set(g.keys())


def terminals(g):
    return symbols(g) - nonterminals(g)


def main(grammar_fn, n):
    grammar = parse_grammar(grammar_fn)
    for i in range(n-1):
        mapping = { sym : '%s_%d' % (sym, i) for sym in nonterminals(grammar) }
        for (lhs, rhs) in list(grammar.items()):
            new_lhs = mapping[lhs]
            new_rhs = [[mapping.get(sym, sym) for sym in rule] for rule in rhs]
            grammar[new_lhs] = new_rhs
        for (s1, s2) in mapping.items():
            grammar[s1].append([s2])
    print_grammar(grammar)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage: ambiguate.py GRAMMAR N')
        exit(1)
    main(sys.argv[1], int(sys.argv[2]))
