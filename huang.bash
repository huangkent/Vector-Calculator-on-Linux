###		Name:	Kent Huang

###  huang.bash
 #
 #   loop:
 #		generates random vectors a, b, & c, and use them to compute vector d 
 #		inputs a, b, & c to a C program, and runs it to compute d
 #		compares the two computed results for d
###


echo

echo "PROMPT:  create random vectors: a, b, & c"
echo "         compute vector d using a, b, & c"
echo

echo Enter
echo "the upper bound of values in vector a (>= 0): "
read UB_a
echo "the lower bound of values in vector a (>= 0): "
read LB_a
echo

echo "the upper bound of values in vector b (>= 0): "
read UB_b
echo "the lower bound of values in vector b (>= 0): "
read LB_b
echo

echo "the upper bound of values in vector c (>= 0): "
read UB_c
echo "the lower bound of values in vector c (>= 0): "
read LB_c
echo

echo "the # of simulations (> 0): "
read M
echo

echo "the vector size (> 0): "
read N
echo

echo --------------------------------------
echo
echo YOUR  INPUTS  ARE
echo
echo "# of simulations = $M"
echo "     vector size = $N"
echo vector a
echo "     Upper Bound = $UB_a"
echo "     Lower Bound = $LB_a"
echo vector b
echo "     Upper Bound = $UB_b"
echo "     Lower Bound = $LB_b"
echo vector c
echo "     Upper Bound = $UB_c"
echo "     Lower Bound = $LB_c"
echo
echo --------------------------------------
echo


# if existing, delete the expected results file (to create a new one)
if [ -f d.txt ]
then
	rm d.txt
fi
# if existing, delete the input file (to create a new one)
if [ -f input.txt ]
then
	rm input.txt
fi
##  if existing, delete the output file (to create a new one)
if [ -f output.txt ]
then
	rm output.txt
fi


##  (1) create the expected results file & the input file
##      d = a + .2 * b - c / 2 is also written into the input file

for ((s = 1; s <= $M; s ++))
do

	# if existing, delete the TEMPORARY data files,
	# which are used to hold a, b, & c created in the current epoch
	if [ -f a.txt ]
	then
		rm a.txt
	fi
	if [ -f b.txt ]
	then
		rm b.txt
	fi
	if [ -f c.txt ]
	then
		rm c.txt
	fi

	##  compute d, and append its value in the expected results file

	for ((i = 1; i <= $N; i ++))
	do
		d_num=.0    # a number in d

		# the ith number of a
		fpnum=`echo | gawk 'BEGIN { srand(systime() + PROCINFO["pid"]); print rand() }'`
		fpnum=$(echo "$LB_a + $fpnum * ($UB_a - $LB_a)" | bc)
		echo "${fpnum}" >> a.txt

		d_num=$fpnum

		# the ith number of b
		fpnum=`echo | gawk 'BEGIN { srand(systime() + PROCINFO["pid"]); print rand() }'`
		fpnum=$(echo "$LB_b + $fpnum * ($UB_b - $LB_b)" | bc)
		echo "${fpnum}" >> b.txt

		d_num=$(echo "$d_num + .2 * $fpnum" | bc)

		# the ith number of c
		fpnum=`echo | gawk 'BEGIN { srand(systime() + PROCINFO["pid"]); print rand() }'`
		fpnum=$(echo "$LB_c + $fpnum * ($UB_c - $LB_c)" | bc)
		echo "${fpnum}" >> c.txt

		# the ith number of d
		d_num=$(echo "$d_num - .5 * $fpnum" | bc)
		echo "${d_num}" >> d.txt		# the expected results file
	done

	echo "" >> d.txt

	##  append the following to the input file:
	##  a, b, c,the formula for d, & 'd'

	exec< a.txt			#  open a.txt
	vector=""
	while read line			#  read the numbers and concatenate them
	do
		vector="$vector$line, "
	done
	length=${#vector}		#  the string length of vector
	vector=${vector: 0: length - 2}	#  cut off the comma in the end of vector
	echo "let a = "$vector >> input.txt

	exec< b.txt			#  open b.txt
	vector=""
	while read line			#  read the numbers and concatenate them
	do
		vector="$vector$line, "
	done
	length=${#vector}		#  the string length of vector
	vector=${vector: 0: length - 2}	#  cut off the comma in the end of vector
	echo "let b = "$vector >> input.txt

	exec< c.txt			#  open c.txt
	vector=""
	while read line			#  read the numbers and concatenate them
	do
		vector="$vector$line, "
	done
	length=${#vector}		#  the string length of vector
	vector=${vector: 0: length - 2}	#  cut off the comma in the end of vector
	echo "let c = "$vector >> input.txt

	echo "let d = a + .2 * b - c / 2" >> input.txt
	echo "d" >> input.txt
	echo ""  >> input.txt

done	# for loop

echo "!" >> input.txt


##  (2) compute d via a program

gcc huang.c -o huang
./huang < input.txt > output.txt    # execute the program with the input file
				    # capture its printout in the output file


##  (3) compare the program and this script for the d values

##  create flag = 1 in a .txt file
if [ -f flag.txt ]
then
	rm flag.txt
fi
echo 1 > flag.txt

##  compare d.txt with output.txt
N=`expr $N + 1`
cnt1=0
cat d.txt |
while read LINE
do

	cnt1=`expr $cnt1 + 1`    # counts the numbers in d.txt
	d=`(echo $LINE)`	 # read a number from the file
	# or use d=`echo $LINE | bc`

	cnt2=0
	cat output.txt |
	while read LINE
	do

		cnt2=`expr $cnt2 + 1`    # counts the numbers in output.txt

		if [ cnt2 == cnt1 ]
		then
			if [ cnt2 % N != 0 ]
			then
				output=`(echo $LINE)`	 # read a number from the file
				# or use output=`echo $LINE | bc`

				if [ $(echo "$output != $d" | bc) -eq 1 ]
				then
					echo 0 > flag.txt
					break
				fi
			fi
		fi

	done

done

##  extract the value from flag.txt
exec< flag.txt
FLAG=1
while read line
do
	FLAG=$line
done

##  check FLAG to determine whether the two files contain the same info
if [ $FLAG == 1 ]
then
	echo "  Conclusion:  SAME"
else
	echo "  Conclusion:  DIFFERENT"
fi
echo

