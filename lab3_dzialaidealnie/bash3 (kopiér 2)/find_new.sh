#!/bin/bash

function vind {
	ret=""
    for f in "$1"/* ; do 
		[[ -d "$f" ]] && vind "$f" $2;
		[[ -f "$f" ]] && [[ "$f" =~ $2 ]] && echo "$f";
	done
}


if [[ $# -eq 0 ]]; then
	x=$(vind . .+)
elif [[ $# -eq 1 ]]; then
	x=$(vind . $1)
else 
	x=$(vind $1 $2)
fi

if [[ "a$x" == "a" ]]; then
	echo "Nic nie znaleziono.";
else
	echo "$x";
fi

