#!/bin/bash

verify() {
    if ./stkn-core.out < samples/$1.stkn > tmp.js && node ./tmp.js | diff samples/$1.expected - ;
    then
        echo $1 "pass."
    else
        echo $1 "FAILED!"
    fi
}

if [ $# == 1 ];
then
    verify $1
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
