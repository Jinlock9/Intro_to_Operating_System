#!/bin/bash

PS3="Please choose one from the above options. :"
select option in "A" "B" "C"
do
	echo
	echo "You choose option ${option}."
	echo
	break
done
