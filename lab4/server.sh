#!/bin/bash

trap "" SIGHUP SIGTERM SIGCHLD
trap "exit 0" SIGUSR1

client=clientfifo;
server=serverfifo;

if [[ -e $server ]]; then
    rm $server;
    mkfifo $server;
    chmod 777 $server;
else
    mkfifo $server;
    chmod 777 $server;
fi

while [[ true ]]; do
    if [[ -z $1 ]]; then
        read line < $server;
        echo $line;
        line=$(echo "$line" | awk '{print $2;}');
        x=$line;
        /bin/bash $0 $x
    else
        x=$1;
        let $[x=x*x];
        echo $x > clientfifo;
        exit 0;
    fi
done
