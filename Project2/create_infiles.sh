#!/bin/bash

diseaseFile=$1
countriesFile=$2
inputDir=$3
numFilesPerDir=$4
numRecordsPerFile=$5

NAMES=("Jack" "Mark" "Sam" "Dave" "Boris" "Joe" "David" 
"Bob" "Eddie" "Chris" "Louis" "Andrew" "Melissa" "Kylie" 
"Mary" "Sofia" "Bella" "Christine" "Anna" "Georgia")
#20 values

LASTNAMES=("Smith" "Jones" "Taylor" "Brown" "Williams" "Wilson" 
"Johnson" "Davies" "Robinson" "Wright" "Thompson" "Evans" "Walker" 
"White" "Roberts" "Green" "Hall" "Wood" "Jackson" "Clarke")
#20 values

echo "${NAMES[19]}"
echo "${LASTNAMES[19]}"

#Create COUNTRIES array
counter=0
input="./$countriesFile"
while IFS= read -r line
do
	#echo "$line"
	COUNTRIES[$counter]=$line
	counter=`expr $counter + 1`

done < "$input"
echo "${COUNTRIES[*]}"

#Create DISEASES array
counter=0
input="./$diseaseFile"
while IFS= read -r line
do
        #echo "$line"
        DISEASES[$counter]=$line
        counter=`expr $counter + 1`

done < "$input"
echo "${DISEASES[*]}"

for i in "${COUNTRIES[@]}"
do
	dir="./$inputDir/$i"
	mkdir $dir
done

#Produce a random lines
a=$numRecordsPerFile
b=0
while [ $b -lt $a ]
do
	record_id=$b
	DIFF=$((19-1+1))
	R=$(($(($RANDOM%$DIFF))+1))
	name=${NAMES[$R]}
        DIFF=$((19-1+1))
        R=$(($(($RANDOM%$DIFF))+1))
	surname=${LASTNAMES[$R]}
        DIFF=$(($counter-1+1))
        R=$(($(($RANDOM%$DIFF))+1))
	virus=${COUNTRIES[$R]}
        DIFF=$((120-1+1))
        R=$(($(($RANDOM%$DIFF))+1))
	age=$R
	line="$record_id ENTER $name $surname $virus $age"
	echo "$line"
	b=`expr $b + 1`
done

