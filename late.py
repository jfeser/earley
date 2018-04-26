#!/usr/bin/env python3

from util import main, State
from random import randint
from queue import Queue

def parse(grammar, sentence):
    work = []
    chart = set()
    completed = set()

    def enqueue(s):
      if s not in chart:
        work.append(s)
      chart.add(s)

    def predict(s):
        completed.add(s)
        for rhs in grammar[s.next_word()]: #can be avoided if we know which symbols were requested at this location
            enqueue(State(s.next_word(), rhs, s.loc))
        for c in [c for c in completed if c.finished() and c.lhs == s.next_word() and c.origin == s.loc]: #can be avoided if we know all the locations where this symbol was parsed originating at this location
            enqueue(s.incr_pos(c.loc))

    def scan(s):
        completed.add(s)
        if s.loc + 1 <= len(sentence) and sentence[s.loc] == s.next_word():
            enqueue(s.incr_pos(s.loc + 1))

    def complete(s): #can be avoided if we know this symbol was already parsed originating and ending at the same origin and location
        completed.add(s)
        for c in [c for c in completed if (not c.finished()) and c.next_word() == s.lhs and c.loc == s.origin]: #can be avoided if we know all the rules at this origin that requested this symbol
            enqueue(c.incr_pos(s.loc))

    for rhs in grammar['START']:
        enqueue(State('START', rhs, 0))

    while work != []:
        s = work.pop(randint(0, len(work) - 1))
        if not s.finished():
            if s.next_word() in grammar:
                predict(s)
            else:
                scan(s)
        else:
            complete(s)
    return chart

if __name__ == '__main__':
    main(parse)
