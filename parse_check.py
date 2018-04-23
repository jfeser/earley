GRAMMAR_FILE = 'arith.gr'
PARSE_FILE = 'arith_parse.test'
START_SYMBOL = 'START'
grammar = {}

def load_grammar():
	with open(GRAMMAR_FILE) as f:
		for line in f.readlines():
			parts = line.split()
			lhs, rhs = parts[0], parts[1:]
			if lhs not in grammar:
				grammar[lhs] = []
			grammar[lhs].append(tuple(rhs))

def check_parses():
	with open(PARSE_FILE) as f:
		for line in f.readlines():
			# Tokenize
			line = ' ( '.join(line.split('('))
			line = ' ) '.join(line.split(')'))
			line = line.split()
			if len(line) == 0: continue
			parsed, i = check_parse(line)
			if i != len(line):
				print("Parsed {} but finished before end of line at {}".format(parsed, i))
			elif parsed == START_SYMBOL:
				print("Parsed")
			else:
				print("Parsed to wrong symbol: {}".format(parsed))

# Return parsed symbol and index to continue from
def check_parse(parse, i=0):
	if parse[i] != '(':
		return (parse[i], i+1)

	i += 1
	lhs = parse[i]
	i += 1
	rhs = []
	while parse[i] != ')':
		symbol, i = check_parse(parse, i)
		if symbol is False:
			return (False, -1)
		rhs.append(symbol)
	i += 1
	rhs = tuple(rhs)
	if lhs in grammar and rhs in grammar[lhs]:
		return (lhs, i)
	else:
		print("Parse failed at {} => {}: {}".format(lhs, len(rhs), rhs))
		return (False, -1)

load_grammar()
check_parses()
