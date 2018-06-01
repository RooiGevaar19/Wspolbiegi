#!/bin/bash

echo "Put number, get number^2"

while true
do
	> output
	read number
	echo $number > ../server/input
	while [[ `cat output`  -eq "" ]]
	do
	:
	done
	cat output
done
exit 0






