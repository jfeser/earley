#!/bin/sh

set -x

# Per-item efficiency for arith grammar.
for i in `seq 1 10`; do
    echo "Arith grammar ambiguity $i";
    ./ambiguate.py arith_testing/arith.gr $i > /tmp/arith-$i.gr;
    numactl -N 1 ./parse -n 20 -p late_parallel /tmp/arith-$i.gr arith_testing/good2.corpus > results/arith_per_item/arith-ambig-$i.csv
    numactl -N 1 ./parse -n 20 -p late_parallel arith_testing/arith-10.gr arith_testing/weak-scaling/good-$i.corpus > results/arith_per_item/arith-length-$i.csv
done

# Strong scaling.
numactl -N 1 ./parse arith_testing/arith-10.gr arith_testing/good2.corpus > results/arith-strong-scaling.csv

numactl -N 1 ./parse java_testing/java_no_epsilons.gr java_testing/elasticsearch.corpus > results/java-strong-scaling.csv

numactl -N 1 ./parse cgw_testing/search.gr cgw_testing/paragraph.corpus > results/cgw-strong-scaling.csv

# Weak scaling.
weak_scaling_bench () {
    GRAMMAR=$1
    OUT_FILE=$2
    BENCH_DIR=$3
    ./parse -n 1 -p late_serial $GRAMMAR $BENCH_DIR/good-1.corpus > $OUT_FILE;
    ./parse -n 1 -p earley_serial $GRAMMAR $BENCH_DIR/good-1.corpus | tail -n +2 >> $OUT_FILE;
    for i in `seq 1 10`; do
        numactl -N 1 ./parse -n $i -p late_parallel $GRAMMAR $BENCH_DIR/good-$i.corpus | tail -n +2 >> $OUT_FILE
    done;
    for i in `seq 11 20`; do
        numactl -N 1 ./parse -n $i -p late_parallel $GRAMMAR $BENCH_DIR/good-10.corpus | tail -n +2 >> $OUT_FILE
    done;

    rm "serial-$OUT_FILE";
    for i in `seq 1 10`; do
        ./parse -n $i -p late_serial $GRAMMAR $BENCH_DIR/good-$i.corpus | tail -n +2 >> $OUT_FILE-serial
    done;
}

echo "Running arith bench..."
weak_scaling_bench arith_testing/arith-10.gr results/arith-weak-scaling.csv arith_testing/weak-scaling;
echo "Running java bench..."
weak_scaling_bench java_testing/java_no_epsilons.gr results/java-weak-scaling.csv java_testing/weak-scaling;
echo "Running cgw bench..."
weak_scaling_bench cgw_testing/search.gr results/cgw-weak-scaling.csv cgw_testing/weak-scaling;

