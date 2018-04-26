code = ''
with open('test.java') as f:
	for line in f.readlines():
		code += ' ' + line + ' '

code = code.replace('(', ' OPEN_PAREN ')
code = code.replace(')', ' CLOSE_PAREN ')
code = code.replace('[', ' OPEN_BRACKET ')
code = code.replace(']', ' CLOSE_BRACKET ')
code = code.replace('{', ' OPEN_BRACE ')
code = code.replace('}', ' CLOSE_BRACE ')
code = code.replace('.', ' . ')
code = code.replace(';', ' ; ')
code = ' '.join(code.split())

print(code)

#Replace all literals (StringLiteral, etc.) and variables
