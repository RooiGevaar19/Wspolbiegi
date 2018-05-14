 #!/bin/bash

dir=`pwd`
echo $dir
for i in `ls $1` ; do
    if [[ -d $i ]] ; then
        echo "$i"
        newdir="$dir/$1/$i"
        echo "$newdir"
        ./find.sh "$newdir"
    fi
done
