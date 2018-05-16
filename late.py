#!/usr/bin/env python3

from util import main, State, multidict
from random import randint
from queue import Queue

def parse(grammar, sentence):
    work = []
    chart = set()
    replies = multidict()
    requests = multidict()
    words = set()

    def enqueue(s):
      if s not in chart:
        work.append(s)
      chart.add(s)

    def predict(s):
        if (s.next_word(), s.loc) not in requests:
            for rhs in grammar[s.next_word()]:
                enqueue(State(s.next_word(), rhs, s.loc))
        requests[(s.next_word(), s.loc)].append(s)
        for loc in replies[(s.next_word(), s.loc)]:
            enqueue(s.incr_word(loc))

    def scan(s):
        if s.loc + 1 <= len(sentence) and sentence[s.loc] == s.next_word():
            enqueue(s.incr_word(s.loc + 1))

    def complete(s):
        if (s.lhs, s.origin, s.loc) not in words: #Should add this to words atomically and do this branch on the result.
            words.add((s.lhs, s.origin, s.loc))
            replies[(s.lhs, s.origin)].append(s.loc)
            for r in requests[(s.lhs, s.origin)]:
                enqueue(r.incr_word(s.loc))

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
