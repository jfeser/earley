#!/usr/bin/env python3

# MIT License

# Copyright (c) 2018 Peter Ahrens, John Feser, Robin Hui

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

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
        #can be avoided if we know which symbols were requested at this location
        for rhs in grammar[s.next_word()]:
            enqueue(State(s.next_word(), rhs, s.loc))
        #can be avoided if we know all the locations where this symbol was parsed originating at this location
        for loc in set([c.loc for c in completed if c.finished() and c.lhs == s.next_word() and c.origin == s.loc]):
            enqueue(s.incr_word(loc))

    def scan(s):
        completed.add(s)
        if s.loc + 1 <= len(sentence) and sentence[s.loc] == s.next_word():
            enqueue(s.incr_word(s.loc + 1))

    #can be avoided if we know this symbol was already parsed originating and ending at the same origin and location
    def complete(s):
        completed.add(s)
        #can be avoided if we know all the rules at this origin that requested this symbol
        for c in [c for c in completed if (not c.finished()) and c.next_word() == s.lhs and c.loc == s.origin]:
            enqueue(c.incr_word(s.loc))

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
