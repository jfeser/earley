#!/bin/bash
#usage: test.sh EARLEY GRAMMAR CORPUS
./reference.py $2 $3 | sort > tmp1
./$1 $2 $3 | sort > tmp2
diff tmp1 tmp2
rm tmp1 tmp2
