#!/usr/bin/env python3

import util
from random import randint

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
        return (self.lhs, self.rhs, self.pos, self.origin, self.loc) == \
            (other.lhs, other.rhs, other.pos, self.origin, self.loc)

    def __hash__(self):
        return hash((self.lhs, self.rhs, self.pos, self.origin, self.loc))

    def finished(self):
        return self.pos == len(self.rhs)

    def next_elem(self):
        return self.rhs[self.pos]

    def incr_pos(self, loc):
        return State(self.lhs, self.rhs, loc, self.origin, self.pos + 1)

def parse(grammar, sentence):
    work = []
    chart = set()
    completed = set()

    def queue(s):
      if s not in chart:
        work.append(s)
      chart.add(s)

    def predict(s):
        completed.add(s)
        if not any((not c.finished()) and c.next_elem() == s.next_elem() and c.loc == s.loc for c in completed if c != s):
            for rhs in grammar[s.next_elem()]:
                queue(State(s.next_elem(), rhs, s.loc))
        else:
            for (lhs, loc, origin) in set([(c.lhs, c.loc, c.origin) for c in completed if c.finished()]):
                if lhs == s.next_elem() and origin == s.loc:
                    queue(s.incr_pos(loc))

    def scan(s):
        completed.add(s)
        if s.loc + 1 <= len(sentence) and sentence[s.loc] == s.next_elem():
            queue(s.incr_pos(s.loc + 1))

    def complete(s):
        if not any(c.lhs == s.lhs and c.origin == s.origin and c.loc == s.loc for c in completed):
            for c in completed:
                if (not c.finished()) and c.next_elem() == s.lhs and c.loc == s.origin:
                    queue(c.incr_pos(s.loc))
        completed.add(s)

    for rhs in grammar['START']:
        queue(State('START', rhs, 0))

    while work != []:
        s = work.pop(randint(0, len(work) - 1))
        if not s.finished():
            if s.next_elem() in grammar:
                predict(s)
            else:
                scan(s)
        else:
            complete(s)
    return chart

if __name__ == '__main__':
    util.main(parse)
