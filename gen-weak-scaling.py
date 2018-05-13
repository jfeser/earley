#!/usr/bin/env python3

import subprocess
import sys
import tempfile

MAX_THREADS=20

def chart_size(grammar, corpus):
    with tempfile.NamedTemporaryFile(mode='w') as f:
        f.write(corpus)
        f.flush()
        out = subprocess.check_output(['./parse', '-n', '1', '-p', 'late_serial', grammar, f.name]).decode('utf-8')
        size = int(out.split('\n')[1].split(',')[-1])
        return size

def binary_search(grammar, words, target_size, low, high):
	if high == low + 1:
		return low
	med = (low + high) // 2
	med_size = chart_size(grammar, ' '.join(words[:med]))
	if med_size > target_size:
		return binary_search(grammar, words, target_size, low, med)
	else:
		return binary_search(grammar, words, target_size, med, high)

grammar = sys.argv[1]
corpus = sys.argv[2]
out_dir = sys.argv[3]

with open(corpus, 'r') as f:
    full_words = f.read().split()

full_size = chart_size(grammar, ' '.join(full_words))
for i in range(1, MAX_THREADS + 1):
    target_size = int(full_size * (i / 20))
    num_words = binary_search(grammar, full_words, target_size, 0, len(full_words))

    out_corpus = ' '.join(full_words[:num_words])
    actual_size = chart_size(grammar, out_corpus)
    print('Target: %d  Got: %d  Error: %d' % (target_size, actual_size, actual_size - target_size))
    with open(out_dir + '/good-%d.corpus' % i, 'w') as f:
        f.write(out_corpus)


