KEYWORDS = 'abstract continue for new switch assert default goto package synchronized boolean do if private this break double implements protected throw byte else import public throws case enum instanceof return transient catch extends int short try char final interface static void class finally long strictfp volatile const float native super while true false null OPEN_PAREN CLOSE_PAREN OPEN_BRACKET CLOSE_BRACKET OPEN_BRACE CLOSE_BRACE < > . ; StringLiteral'.split()

# Changes literals and identifiers to the appropriate symbol.
def to_symbol(word):
	#FIXME: Integer, Float, Character, Boolean, Null literals
	if word in KEYWORDS:
		return word
	print(word)
	return 'IDENTIFIER'

def read_file(filename):
	code = ''
	with open(filename) as f:
		for line in f.readlines():
			code += ' ' + line + ' '
	return code

code = read_file('test.java')

# Replace symbols with names
code = code.replace('(', ' OPEN_PAREN ')
code = code.replace(')', ' CLOSE_PAREN ')
code = code.replace('[', ' OPEN_BRACKET ')
code = code.replace(']', ' CLOSE_BRACKET ')
code = code.replace('{', ' OPEN_BRACE ')
code = code.replace('}', ' CLOSE_BRACE ')

# Add spaces between symbols
code = code.replace('.', ' . ') # FIXME: messes up float literals
code = code.replace(';', ' ; ')

# Replace string literals
while code.find('"') != -1:
	first = code.find('"')
	second = code.find('"', first+1)
	if second == -1:
		print("Unmatched parentheses")
		print(code)
		exit()
	code = code[:first] + " StringLiteral " + code[second+1:]

# Convert literals, variables, etc. to symbols
code = [to_symbol(w) for w in code.split()]
code = ' '.join(code)

print(code)
