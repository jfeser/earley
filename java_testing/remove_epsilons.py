import random

def with_nulls(lhs, full_rhs, nullable):
	all_rhs = []
	#print('{:80s}{}'.format(lhs, ' '.join(full_rhs)))
	#print("to:")
	for rhs in recursive_nulls(full_rhs, nullable):
		if len(rhs) > 0:
			all_rhs.append((lhs, rhs))
			#print('{:80s}{}'.format(lhs, ' '.join(rhs)))
	#print()
	return all_rhs

def recursive_nulls(rhs, nullable):
	if len(rhs) == 0:
		yield []
		return
	symbol = rhs[0]
	if symbol in nullable:
		for next_rhs in recursive_nulls(rhs[1:], nullable):
			yield [symbol] + next_rhs
			yield next_rhs
	else:
		for next_rhs in recursive_nulls(rhs[1:], nullable):
			yield [symbol] + next_rhs

in_grammar = []
with open('java_formatted.gr') as f:
	for line in f.readlines():
		line = line.strip().split()
		if len(line) == 0: continue
		in_grammar.append((line[0], line[1:]))

# find nullables
nullable = set()

for _ in range(10):
	found = 0
	for lhs, rhs in in_grammar:
		if lhs in nullable: continue
		lhs_nullable = True
		for symbol in rhs:
			if symbol not in nullable:
				lhs_nullable = False
		if lhs_nullable:
			nullable.add(lhs)
			found += 1

grammar = []
for lhs, rhs in in_grammar:
	grammar += with_nulls(lhs, rhs, nullable)

for lhs, rhs in grammar:
	print('{:80s}{}'.format(lhs, ' '.join(rhs)))
