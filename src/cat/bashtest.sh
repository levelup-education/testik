#!/bin/bash

FLAGS=(
    ""
    "-b"
    "-e"
    "-n"
    "-s"
    "-t"
    "-be"
    "-bn"
    "-ns"
    "-bet"
)

for flag in "${FLAGS[@]}"
do
    diff <(cat $flag test.txt) \
         <(./s21_cat $flag test.txt)

    if [ $? -eq 0 ]
    then
        echo "PASS: $flag"
    else
        echo "FAIL: $flag"
    fi
done