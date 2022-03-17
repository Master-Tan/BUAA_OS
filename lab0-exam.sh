#!/bin/bash
make
touch hello_os
mkdir hello_os_dir
cp os_hello ./hello_os_dir/
cp os_hello ./hello_os_dir/hello_os
rm os_hello
touch hello_os.txt
grep -i -n "os_hello" hello_os.c > hello_os.txt
