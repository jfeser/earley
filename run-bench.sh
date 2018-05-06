#!/bin/sh

for i in `seq 2 12`; do
    echo "Arith grammar ambiguity $i";
    ./ambiguate.py arith_testing/arith.gr $i > arith-$i.gr;
    numactl -N 1 ./parse arith-$i.gr arith_testing/good2.corpus > arith-$i.csv
done

