with open('cgw.corpus') as f:
	corpus = [line.strip() for line in f.readlines()]

clean_corpus = []
with open('success.txt') as f:
	i = 0
	for line in f.readlines():
		if line.strip() != 'failure':
			clean_corpus.append(corpus[i])
		i += 1

print(corpus[:5])
print(clean_corpus[:5])
print(len(corpus))
print(len(clean_corpus))

for line in clean_corpus:
	print(line)