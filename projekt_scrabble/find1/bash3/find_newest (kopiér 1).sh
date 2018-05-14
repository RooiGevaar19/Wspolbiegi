#!/bin/bash
declare -i ilosc_plikow
ilosc_plikow=0
potomne=""

#if [ "$#" -lt 2 ]; then
#	echo 'Za mało argumentów.'
#	echo 'Wywołanie programu: '$0' [katalog] [regex].'
#	exit
#else
#	if [ ! -d "$1" ]; then
#		echo "[PORAŻKA]: Katalog o nazwie '$1' nie został znaleziony."
#		exit
#	fi
#fi

if [[ $# -eq 0 ]]; then A="."; B=.+; C="$3";
elif [[ $# -eq 1 ]]; then A="."; B="$1"; C="$3";
else A="$1"; B=$2; C="$3";
fi

for i in "$A"/*; do
	if [ -d "$i" ]; then
		"$0" "$i" $B "notmain" &
		potomne+=("$!")
	elif [ -f "$i" ]; then
		if [[ "$i" =~ $B ]] ; then
			ilosc_plikow=$((ilosc_plikow+1)); echo "$i";
		fi
	fi
done

for i in "${potomne[@]}"; do
	if [ "$i" ]; then
		wait "$i"
		ilosc_plikow=$((ilosc_plikow+$?))
	fi
done

if [ "a$C" == "a" ]; then
	if [ "$ilosc_plikow" -eq 0 ]; then
		echo "Nie znaleziono."
	fi
fi

exit "$ilosc_plikow"