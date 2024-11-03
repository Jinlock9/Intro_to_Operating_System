#!/bin/bash

for i in {1..10}
do
    ./ex2.sh; ./ex2.sh&
    #./ex2_modified.sh; ./ex2_modified.sh&
done