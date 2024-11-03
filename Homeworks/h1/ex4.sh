#!/bin/sh

#1
user add -m jinlock
#2
ps -ael
#3
cat /proc/cpuinfo
cat /proc/meminfo
#4
head -n 1 /dev/random | tee 1.txt > 2.txt
#5
cat 1.txt 2.txt > 3.txt
#6
hexdump 3.txt
#7
find . -name "*mutex*" | xargs grep -rlw "nest_lock"