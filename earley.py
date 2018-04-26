#!/usr/bin/env python3

from util import main, State
from random import randint
from queue import PriorityQueue

def parse(grammar, sentence):
    work = PriorityQueue()
    chart = set()

    def enqueue(s):
      if s not in chart:
        work.put((s.loc, s))
      chart.add(s)

    def predict(s):
        for rhs in grammar[s.next_word()]:
            enqueue(State(s.next_word(), rhs, s.loc))

    def scan(s):
        if s.loc + 1 <= len(sentence) and sentence[s.loc] == s.next_word():
            enqueue(s.incr_pos(s.loc + 1))

    def complete(s):
        for c in [c for c in chart if (not c.finished()) and c.next_word() == s.lhs and c.loc == s.origin]:
            enqueue(c.incr_pos(s.loc))

    for rhs in grammar['START']:
        enqueue(State('START', rhs, 0))

    while not work.empty():
        (loc, s) = work.get()
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
