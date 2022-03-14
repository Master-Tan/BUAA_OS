#!/bin/bash
#20373864
lines=`sed "s/${2}/${3}/g" ${1}`
echo "$lines" > ${1}
