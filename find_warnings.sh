#!/bin/bash
gcc -Wall ${1} -o test 2> warning.txt
b=$?
grep "warning" warning.txt > tanlide.txt
sed 's/warning: //g' tanlide.txt >result.txt
rm tanlide.txt
a=1
if [ $b -eq 0 ]
then
while [ $a -le ${2} ]
do
echo $a | ./test >> result.txt
a=$[$a+1]
done
fi
pwd >> result.txt


