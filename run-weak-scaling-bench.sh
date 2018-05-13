#!/bin/sh

weak-scaling-bench () {
    GRAMMAR=$1
    OUT_FILE=$2
    BENCH_DIR=$3
    ./parse -n 1 -p late_serial $GRAMMAR $BENCH_DIR/good-1.corpus > $OUT_FILE;
    ./parse -n 1 -p earley_serial $GRAMMAR $BENCH_DIR/good-1.corpus | tail -n +2 >> $OUT_FILE;
    for i in `seq 1 20`; do
        numactl -N 1 ./parse -n $i -p late_parallel $GRAMMAR $BENCH_DIR/good-$i.corpus | tail -n +2 >> $OUT_FILE
    done;
}

echo "Running arith bench..."
weak-scaling-bench arith_testing/arith-10.gr arith-weak-scaling.csv arith_testing/weak-scaling;
echo "Running java bench..."
weak-scaling-bench java_testing/java_no_epsilons.gr java-weak-scaling.csv java_testing/weak-scaling;
echo "Running cgw bench..."
weak-scaling-bench cgw_testing/search.gr cgw-weak-scaling.csv cgw_testing/weak-scaling;
