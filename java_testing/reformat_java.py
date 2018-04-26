import random

def process(grammar, rule, i=0, end=None):
	# Given a rule, adds auxiliary rules to the grammar to remove "[ { (" symbols.
	new_rule = []
	while i < len(rule):
		symbol = rule[i]
		i += 1
		if symbol == end:
			return (i, new_rule)
			
		if symbol == '{':
			i, subrule = process(grammar, rule, i, '}')
			name = 'Many' + ''.join(subrule) + str(random.randrange(100)) # Strip non-alphabetical characters?
			grammar.append((name, []))
			grammar.append((name, subrule + [name]))
			new_rule.append(name)
		elif symbol == '[':
			i, subrule = process(grammar, rule, i, ']')
			name = 'Maybe' + ''.join(subrule) + str(random.randrange(100)) # Strip non-alphabetical characters?
			grammar.append((name, []))
			grammar.append((name, subrule))
			new_rule.append(name)
		else:
			new_rule.append(symbol)

	if end is not None:
		print("Error on {}".format(rule))
	return (i, new_rule)

in_grammar = []
with open('java_cleaned.gr') as f:
	lhs = None
	rhs = []
	for line in f.readlines():
		line = line.strip()
		if len(line) == 0: continue
		if line[-1] == ':':
			# New lhs
			lhs = line[:-1]
		else:
			# New rule
			in_grammar.append((lhs, line.split()))

grammar = []
for lhs, rhs in in_grammar:
	_, new_rule = process(grammar, rhs)
	grammar.append((lhs, new_rule))

for lhs, rhs in grammar:
	print('{:80s}{}'.format(lhs, ' '.join(rhs)))
