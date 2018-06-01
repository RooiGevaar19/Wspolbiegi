#!/bin/bash

declare -i count;
count=0;
potomne="";

# A - sciezka
# B - wzorzec
# C - flaga dla programu
if [[ $# -eq 0 ]]; then A="."; B=.+; C="$3";
elif [[ $# -eq 1 ]]; then A="."; B="$1"; C="$3";
else A="$1"; B=$2; C="$3";
fi

for i in "$A"/*; do
	if [[ -d "$i" ]] ; then
		"$0" "$i" $B "notmain" & potomne+=("$!");
	elif [[ -f "$i" && "$i" =~ $B ]] ; then
		count=$[$count + 1]; 
		echo "$i";
	fi
done

for i in "${potomne[@]}"; do
	if [ "$i" ]; then
		wait "$i"; 
		count=$[count + $?]; 
	fi
done

if [[ "a$C" == "a" && "$count" -eq 0 ]] ; then
	echo "Nie znaleziono."; fi

exit "$count";