#!/bin/bash
# 20373864
touch $2
sed -n '8p' $1 >> $2
sed -n '32p' $1 >> $2
sed -n '128p' $1 >> $2
sed -n '512p' $1 >> $2
sed -n '1024p' $1 >> $2

