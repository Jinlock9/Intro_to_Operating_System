#!/bin/bash

FILE=./ex2.out
if ! [ -f $FILE ]; then
    echo 0 >> $FILE
fi

for i in {1..10}
do
    op=$(tail -n 1 $FILE)
    ((op++))
    echo $op >> $FILE
done