#!/bin/bash

if [[ $# -eq 0 ]]; then
	x=`./vind . .*`
elif [[ $# -eq 1 ]]; then
	x=`./vind . $1`
else 
	x=`./vind $1 $2`
fi

if [[ "a$x" == "a" ]]; then
	echo "Nic nie znaleziono.";
else
	echo "$x";
fi
