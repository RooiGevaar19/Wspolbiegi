#!/bin/bash

user=$(whoami);
client=clientfifo;

if [[ -e $client ]] ; then
	rm $client;
	mkfifo $client;
	chmod 777 $client;
else
	mkfifo $client;
	chmod 777 $client;
fi

echo -e "\e[1;36m$user\e[m:\t$1" > serverfifo;
while [[ $line -le 0 ]] ; do
	read line < $client;
	echo $line;
done
exit 0;
