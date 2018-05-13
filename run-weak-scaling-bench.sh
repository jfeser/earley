#!/bin/sh

# Arith benchmark
GRAMMAR=arith_testing/arith.gr
OUT_FILE=arith-weak-scaling.csv
BENCH_DIR=arith_testing/weak-scaling
./parse -n 1 -p late_serial $GRAMMAR $BENCH_DIR/good-1.txt > $OUT_FILE;
./parse -n 1 -p earley_serial $GRAMMAR $BENCH_DIR/good-1.txt | tail -n +2 >> $OUT_FILE;
for i in `seq 1 20`; do
    numactl -N 1 ./parse -n $i -p late_parallel $GRAMMAR $BENCH_DIR/good-$i.txt | tail -n +2 >> $OUT_FILE
done;

# Java benchmark
GRAMMAR=java_testing/java_no_epsilons.gr
OUT_FILE=java-weak-scaling.csv
BENCH_DIR=java_testing/weak-scaling
./parse -n 1 -p late_serial $GRAMMAR $BENCH_DIR/good-1.corpus > $OUT_FILE;
./parse -n 1 -p earley_serial $GRAMMAR $BENCH_DIR/good-1.corpus | tail -n +2 >> $OUT_FILE;
for i in `seq 1 20`; do
    numactl -N 1 ./parse -n $i -p late_parallel $GRAMMAR $BENCH_DIR/good-$i.corpus | tail -n +2 >> $OUT_FILE
done;

# CGW benchmark
GRAMMAR=cgw_testing/search.gr
OUT_FILE=cgw-weak-scaling.csv
BENCH_DIR=cgw_testing/weak-scaling
./parse -n 1 -p late_serial $GRAMMAR $BENCH_DIR/good-1.txt > $OUT_FILE;
./parse -n 1 -p earley_serial $GRAMMAR $BENCH_DIR/good-1.txt | tail -n +2 >> $OUT_FILE;
for i in `seq 1 20`; do
    numactl -N 1 ./parse -n $i -p late_parallel $GRAMMAR $BENCH_DIR/good-$i.txt | tail -n +2 >> $OUT_FILE
done;

