#!/bin/bash
#First you can use grep (-n) to find the number of lines of string.
# 20373864
tofile=${3}
grep -n ${2} ${1} > 1.txt
#Then you can use awk to separate the answer.
awk -F ':' '{print $1}' 1.txt > $tofile
if [ "$tofile" != "1.txt" ]
then
	rm 1.txt
fi
