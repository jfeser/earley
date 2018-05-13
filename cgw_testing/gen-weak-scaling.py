#!/usr/bin/env python3

with open('paragraph.corpus', 'r') as f:
    words = f.read().split(' ')

for i in range(1, 21):
    num_words = len(words)//(20-i+1)
    print('Writing %d words.' % num_words)
    with open('weak-scaling/good-%d.txt' % i, 'w') as f:
        f.write(' '.join(words[:num_words]))
