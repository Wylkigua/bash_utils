#!/bin/bash


SUCCESS=0
FAIL=0
COUNTER=0
DIFF=""

s21_cmd=("./s21_cat")
s21_log=("s21.log")
sys_cmd=("cat")
sys_log=("cat.log")

flags=(
	"b"
	"e"
	"s"
	"n"
	"t"
	"T"
	"E"
	"-number"
	"-squeeze-blank"
	"-number-nonblank"
)

files=(
	"test_files/test_1_cat.txt"
	"test_files/test_2_cat.txt"
	"test_files/test_3_cat.txt"
	"test_files/test_4_cat.txt"
	"test_files/test.txt"
	"test_files/test2.txt"
#	"test_files/test_1_cat.txt test_files/test_2_cat.txt"
#	"test_files/test_2_cat.txt test_files/test_3_cat.txt"
#	"test_files/test_3_cat.txt test_files/test_4_cat.txt"
)

run() {
	let "COUNTER++"
	"${s21_cmd[@]}" $1 $2 > "${s21_log[@]}"
	"${sys_cmd[@]}" $1 $2 > "${sys_log[@]}"
    #valgrind --tool=memcheck --leak-check=full --error-exitcode=1 ${s21_cmd[@]} $1 $2 >> test_s21_cat.log 2>&1
	DIFF="$(diff -s "${s21_log[@]}" "${sys_log[@]}")"
	result=$?
	
	if [ $result -eq 0 ]
	then 
		let "SUCCESS++"
        echo -e "\e[32m#$COUNTER ::SUCCESS:: CMD: $s21_cmd PARAM: $1 FILE: $2\e[0m"
	elif [ $result -eq 1 ]
	then
		let "FAIL++"
		echo -e "\e[31m#$COUNTER ::FAIL:: CMD: $s21_cmd PARAM: $1 FILE: $2\e[0m"
	else
		echo "WARNING: wrong with the diff command"
	fi
	rm -f "${s21_log[@]}" "${sys_log[@]}"
}

echo "RUN TEST"
printf "\n"

for f in "${flags[@]}"
do
	for i in "${files[@]}"
	do
		run "-$f" "$i"
	done

done

printf "\n"
echo -e "\e[32mSUCCESS: $SUCCESS\e[0m"
echo -e "\e[31mFAIL: $FAIL\e[0m"
echo "ALL: $COUNTER"
