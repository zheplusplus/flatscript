#!/bin/bash

if [ "-cm" = "$1" ];
then
    CHKMEM="valgrind --log-file=tmp.log.memcheck --leak-check=full"
    CASE=$2
else
    CASE=$1
fi

verify() {
    if $CHKMEM ./stekin < samples/$1.stkn | node | diff samples/$1.expected - ;
    then
        echo $1 "pass."
    else
        echo $1 "FAILED!"
    fi
}

if [ $CASE ];
then
    verify $CASE
    exit
fi

echo "sample-test:"

verify empty
verify write
verify latency-ref
verify fib
verify nest-func
verify return-void
verify pair
verify sqrt
verify find-root
verify fixed-point
verify vector-multi
verify big-literals
verify basic-list
verify return-list
verify list-pipe
verify string
verify lookup
verify list-slice
verify qsort
verify set-attribute
verify dict
verify lambda
verify set-timeout
verify async-calls
verify async-pipes
verify ref-this
verify async-fib
