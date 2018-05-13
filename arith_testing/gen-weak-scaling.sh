#!/usr/bin/env bash

FACTOR=50
SLOP=5

for i in `seq 1 20`; do
    let min_size=i*FACTOR
    let max_size=min_size+SLOP
    echo "Generating size [$min_size, $max_size]";
    ../randsent --min-size $min_size --max-size $max_size arith.gr > weak-scaling/good-$i.txt
done
