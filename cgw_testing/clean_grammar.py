with open('S1_GRAMMAR.gr') as f:
	for line in f:
		line = line.strip()
		if len(line) == 0: continue
		if line[0] == '#': continue
		line = line.split()[1:]
		if len(line) < 2:
			print(line)
			break
		print(' '.join(line))
