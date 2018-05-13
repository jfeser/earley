#!/bin/sh

echo "Generating Java benchmarks..."
./gen-weak-scaling.py java_testing/java_no_epsilons.gr java_testing/elasticsearch.corpus java_testing/weak-scaling/ &
echo "Generating arith benchmarks..."
./gen-weak-scaling.py arith_testing/arith.gr arith_testing/good2.corpus arith_testing/weak-scaling/ &
echo "Generating CGW benchmarks..."
./gen-weak-scaling.py cgw_testing/search.gr cgw_testing/paragraph.corpus cgw_testing/weak-scaling/ &
wait
