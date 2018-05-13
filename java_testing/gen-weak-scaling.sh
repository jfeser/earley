#!/bin/bash

INPUT=elasticsearch.corpus

cp $INPUT weak-scaling/good-1.corpus;
last=weak-scaling/good-1.corpus;
for i in `seq 2 20`; do
    next="weak-scaling/good-$i.corpus";
    cat $INPUT $last | tr -d "\n" > $next;
    last=$next 
done;
