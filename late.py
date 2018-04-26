#!/usr/bin/env python3

from util import main, State
from random import randint
from queue import Queue

def parse(grammar, sentence):
    work = Queue()
    chart = set()
    completed = set()

    def enqueue(s):
      if s not in chart:
        work.put(s)
      chart.add(s)

    def predict(s):
        completed.add(s)
        if not any((not c.finished()) and c.next_word() == s.next_word() and c.loc == s.loc for c in completed if c != s):
            for rhs in grammar[s.next_word()]:
                enqueue(State(s.next_word(), rhs, s.loc))
        else:
            for (lhs, loc, origin) in set([(c.lhs, c.loc, c.origin) for c in completed if c.finished()]):
                if lhs == s.next_word() and origin == s.loc:
                    enqueue(s.incr_pos(loc))

    def scan(s):
        completed.add(s)
        if s.loc + 1 <= len(sentence) and sentence[s.loc] == s.next_word():
            enqueue(s.incr_pos(s.loc + 1))

    def complete(s):
        if not any(c.lhs == s.lhs and c.origin == s.origin and c.loc == s.loc for c in completed):
            for c in completed:
                if (not c.finished()) and c.next_word() == s.lhs and c.loc == s.origin:
                    enqueue(c.incr_pos(s.loc))
        completed.add(s)

    for rhs in grammar['START']:
        enqueue(State('START', rhs, 0))

    while work.full():
        s = work.get()
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
