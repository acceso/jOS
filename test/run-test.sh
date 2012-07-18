#!/bin/sh


test -f ${1}.c || { echo "No such test."; exit 1; }


if [ -f $1.cflags ]; then 
	EXTRA=`cat $1.cflags`
else
	EXTRA=""
fi


gcc -DTESTING -nostdinc -I. -I../ -I../include -std=gnu99 \
	-ggdb -O0 -Wall -o $1 $EXTRA $1.c \
|| exit 1


./$1 | diff -w -B -I "^#" $1.t -

[ $? -eq 0 ] || { echo "\n*** Error en test: $1 ***\n"; exit 1; }
 
exit 0


