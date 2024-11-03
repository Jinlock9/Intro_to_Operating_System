#!/bin/bash


ans=$((($RANDOM)%100))
while true ; do
	read -p "Please guess: " guess
	if [ $ans -eq $guess ] ; then
		echo "You Win!"
		break
	elif [ $ans -gt $guess ] ; then
		echo "Larger"
	else
		echo "Smaller"
	fi
done
