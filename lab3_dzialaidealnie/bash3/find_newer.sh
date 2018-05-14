#!/bin/bash

function vind {
	potomne=""
	for f in "$1"/* ; do 
		if [[ -d "$f" ]] ; then 
			vind "$f" $2 & 
			potomne+=("$!")
		fi
		[[ -f "$f" && "$f" =~ $2 ]] && echo "$f"; 
		echo "${potomne[@]}";
	done


	for i in "${potomne[@]}"; do
		if [ "$i" ]; then wait "$i"; fi
	done
}


if [[ $# -eq 0 ]]; then x=$(vind . .+ &)
elif [[ $# -eq 1 ]]; then x=$(vind . $1 &)
else x=$(vind $1 $2 &)
fi

if [[ "pusto$x" == "pusto" ]]; then
echo "Nic nie znalazlem. XD"; exit;
else echo "$x"; exit; fi

