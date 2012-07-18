#!/bin/sh


for a in *.c ; do
	TEST=`basename $a .c`
	./run-test.sh $TEST
	RES=$?
	if [ $RES -ne 0 ]; then
		exit 1
	else
		echo "Test: $TEST... OK"
	fi
done


echo "\nAll ok :)"


