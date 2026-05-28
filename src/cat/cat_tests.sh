#!/bin/bash

GREEN="\033[32m"
RED="\033[31m"
RESET="\033[0m"

SUCCESS=0
FAIL=0
COUNTER=0

S21_COMM="./s21_cat"
SYS_COMM="cat"

S21_LOG="s21_cat.log"
SYS_LOG="sys_cat.log"

tests=(
"FLAGS cat_test_files/test_case_cat.txt"
"FLAGS cat_test_files/test_case_cat.txt cat_test_files/test_1_cat.txt"
)
flags=(
    "b"
    "e"
    "n"
    "s"
    "t"
    "v"
)
manual=(
"-s cat_test_files/test_1_cat.txt"
"-b -e -n -s -t -v cat_test_files/test_1_cat.txt"
"-b cat_test_files/test_1_cat.txt nofile.txt"
"-t cat_test_files/test_3_cat.txt"
"-n cat_test_files/test_2_cat.txt"
"no_file.txt"
"-n -b cat_test_files/test_1_cat.txt"
"-s -n -e cat_test_files/test_4_cat.txt cat_test_files/test_6_cat.txt cat_test_files/test_7_cat.txt"
"cat_test_files/test_1_cat.txt -n"
"-n cat_test_files/test_1_cat.txt"
"-n cat_test_files/test_1_cat.txt cat_test_files/test_2_cat.txt"
"-v cat_test_files/test_5_cat.txt"
"-- cat_test_files/test_5_cat.txt"
"-Z test2.txt -n test1.txt"
"-s test0.txt"
"-s test1.txt no_file.txt"
)

gnu=(
"-T cat_test_files/test_1_cat.txt"
"-E cat_test_files/test_1_cat.txt"
"-TE cat_test_files/test_1_cat.txt"
"-vT cat_test_files/test_3_cat.txt"
"--number cat_test_files/test_2_cat.txt"
"--squeeze-blank cat_test_files/test_1_cat.txt"
"--squeeze-blank -TE test_1_cat.txt"
"--number-nonblank cat_test_files/test_4_cat.txt cat_test_files/test_9_cat.txt cat_test_files/test_10_cat.txt"
"cat_test_files/test_1_cat.txt --number --number"
"-benstv cat_test_files/test_6_cat.txt"
"--incorrect_flag test2.txt -n test1.txt"
)

run_test() {
    param=$(echo "$@" | sed "s/FLAGS/$var/")

    "$S21_COMM" $param > "$S21_LOG" 2>&1
    "$SYS_COMM" $param > "$SYS_LOG" 2>&1

    let "COUNTER++"

    if diff "$S21_LOG" "$SYS_LOG" > /dev/null; then
        let "SUCCESS++"
    else
        let "FAIL++"
        echo -e "${RED}FAIL${RESET}: cat $param"
        echo "Diff:"
        diff "$S21_LOG" "$SYS_LOG" | head -10
        echo "---"
    fi
    rm -f "$S21_LOG" "$SYS_LOG"
}

echo "############################################"
echo "CAT UTILITY TESTS FOR ANY FLAG COMBINATIONS"
echo "############################################"
printf "\n"
printf "\n"
# ручные тесты
for i in "${manual[@]}"
do
    var="-"
    run_test "$i"
done
printf "\n"
# 1 флаг
printf "\n"
for var1 in "${flags[@]}"
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        run_test "$i"
    done
done
printf "\n"
# 2 флага
printf "\n"
for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                run_test "$i"
            done
        fi
    done
done
printf "\n"
# 3 флага
printf "\n"
for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        for var3 in "${flags[@]}"
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    run_test "$i"
                done
            fi
        done
    done
done
printf "\n"
# 4 флага
printf "\n"
for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        for var3 in "${flags[@]}"
        do
            for var4 in "${flags[@]}"
            do
                if [ $var1 != $var2 ] && [ $var2 != $var3 ] \
                && [ $var1 != $var3 ] && [ $var1 != $var4 ] \
                && [ $var2 != $var4 ] && [ $var3 != $var4 ]
                then
                    for i in "${tests[@]}"
                    do
                        var="-$var1 -$var2 -$var3 -$var4"
                        run_test "$i"
                    done
                fi
            done
        done
    done
done
# сдвоенные флаги
for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1$var2"
                run_test "$i"
            done
        fi
    done
done

# строенные флаги
for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        for var3 in "${flags[@]}"
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1$var2$var3"
                    run_test "$i"
                done
            fi
        done
    done
done
printf "\n"
echo -e "FAILED: $RED $FAIL $RESET"
echo -e "SUCCESSFUL: $GREEN $SUCCESS $RESET"
echo -e "ALL: $COUNTER"
printf "\n"
echo "#################################"
echo "CAT UTILITY TESTS WITH GNU FLAGS"
echo "#################################"
printf "\n"
FAIL=0
SUCCESS=0
COUNTER=0
printf "\n"

for i in "${gnu[@]}"
do
    var="-"
    run_test $i
done
printf "\n"

echo -e "FAILED: $RED $FAIL $RESET"
echo -e "SUCCESSFUL: $GREEN $SUCCESS $RESET"
echo -e "ALL: $COUNTER"
printf "\n"