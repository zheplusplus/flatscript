#!/bin/bash

if [ "-cm" = "$1" ];
then
    CHKMEM="valgrind --log-file=tmp.log.memcheck --leak-check=full"
    CASE=$2
else
    CASE=$1
fi

verify() {
    if $CHKMEM ./flatsc < samples/errors/$1.fls 2>&1 > /dev/null | diff samples/errors/$1.expected - ;
    then
        echo $1 "report pass."
    else
        echo $1 "report FAILED!"
    fi
}

if [ $CASE ];
then
    verify $CASE
    exit
fi

echo "sample-report-test:"

verify inv-chars
verify bad-indent
verify tab-as-ind
verify reserved-words
verify slice
verify statments-after-return
verify super-ctor-without-base-class
