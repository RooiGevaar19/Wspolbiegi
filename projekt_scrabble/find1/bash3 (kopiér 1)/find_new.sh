#!/bin/bash

function vind {
	for f in "$1"/* ; do 
		if [[ -d "$f" ]] ; then vind "$f" $2 & fi
		if [[ -f "$f" && "$f" =~ $2 ]] ; then 
		echo "$f"; fi
	done
}


if [[ $# -eq 0 ]]; then x=$(vind . .+ &)
elif [[ $# -eq 1 ]]; then x=$(vind . $1 &)
else x=$(vind $1 $2 &)
fi

if [[ "pusto$x" == "pusto" ]]; then
echo "Nic nie znalazlem. XD"; exit;
else echo "$x"; exit; fi

