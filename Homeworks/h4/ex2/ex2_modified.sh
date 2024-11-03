#!/bin/bash

FILE=./ex2_modified.out
if ! [ -f "$FILE" ]; then
    echo 0 >> $FILE
fi

for i in {1..10}
do (
    flock -n -x 99 || exit 1
    op=$(tail -n 1 $FILE)
    ((op++))
    echo $op >> $FILE
) 99>>$FILE
done