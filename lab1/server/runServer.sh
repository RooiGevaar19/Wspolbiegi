#!/bin/bash

echo "PLDL Server started"
echo "Listening on input"

while true
do
	while [[ `cat input` -eq "" ]]
	do
	:
	done
	echo "=======NEW REQUEST========"
	echo "Time: "`date +"%T"`
	number=$( cat input )
	echo "Retrieved: "$number	
	number=$[$number * $number]
	echo $number > ../client/output
	echo "Sent: "$number
	> input

done
exit 0
