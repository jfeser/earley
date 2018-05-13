import subprocess

#python 3.5.2
TARGET_CHART_SIZE = 7400
CORPUS_FILE = 'java_testing/elasticsearch.corpus'
TEMP_FILE = 'java_testing/temp.corpus'
GRAMMAR_FILE = "./java_testing/java_no_epsilons.gr"

with open(CORPUS_FILE) as f:
	full_corpus = f.read().split()

def chart_size(input_size):
	corpus = full_corpus[:input_size]
	with open(TEMP_FILE, 'w') as f:
		f.write(' '.join(corpus))
	val = subprocess.run(["python", "earley.py", GRAMMAR_FILE, TEMP_FILE], stdout=subprocess.PIPE)
	output = val.stdout.decode('utf-8')
	return output.count('\n')

def binary_search(target_size, low=0, high=len(full_corpus)):
	if high == low + 1:
		return low
	med = (low + high) // 2
	med_size = chart_size(med)
	print("{} {} => {}".format(low, high, med_size))

	if med_size > target_size:
		return binary_search(target_size, low, med)
	else:
		return binary_search(target_size, med, high)

size = binary_search(TARGET_CHART_SIZE)
print(' '.join(full_corpus[:size]))
