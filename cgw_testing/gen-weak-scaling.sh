#!/bin/bash

cp paragraph.corpus weak-scaling/good-1.txt;
last=weak-scaling/good-1.txt;
for i in `seq 2 20`; do
    next="weak-scaling/good-$i.txt";
    cat paragraph.corpus $last | tr -d "\n" > $next;
    last=$next
done;
