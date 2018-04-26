#!/bin/bash
#usage: test.sh EARLEY GRAMMAR CORPUS
python3 reference.py $2 $3 | sort > tmp1
python3 $1 $2 $3 | sort > tmp2
diff tmp1 tmp2
rm tmp1 tmp2
