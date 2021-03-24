#! /bin/bash

virusFile=$1
counrtyFile=$2
num=$3
duplicates=$4

declare -a arrCountries;
declare -a arrViruses;
declare -A arrayAlreadyCreated;

while IFS= read -r line; do
    arrCountries+=("$line")
done < $counrtyFile

while IFS= read -r line; do
    arrViruses+=("$line")
done < $virusFile

lengthOfCountries=${#arrCountries[@]}
lengthOfViruses=${#arrViruses[@]}

for (( i=0; i<$num; i++ )) do
    lengthOfarrayAlreadyCreated=${#arrayAlreadyCreated[@]}
    id=0
    if [ $duplicates -eq 1 ]    
    then
        if [ $lengthOfarrayAlreadyCreated -eq 0 ]
        then
            id=$((1 + $RANDOM % 9999))
        else
            possibility=4
            if [ $((1 + $RANDOM % 10)) -lt $possibility ]
            then
                indexarrayAlreadyCreated=$((0 + $RANDOM % $lengthOfarrayAlreadyCreated))
                id=${arrayAlreadyCreated[indexarrayAlreadyCreated]}
                if [ $((1 + $RANDOM % 10)) -lt $possibility ]
                then
                    id=${arrayAlreadyCreated[lengthOfarrayAlreadyCreated-1]}
                fi
            fi
            
        fi
    fi

    if [ $id -eq 0 ]
    then
        flag=1
        while [ $flag -ne 0 ]
        do
            id=$((1 + $RANDOM % 9999))
            flag=0
            for (( j=0; j<$lengthOfarrayAlreadyCreated; j++ )) do
                if [ ${arrayAlreadyCreated[j]} -eq $id ]
                then
                    flag=1
                    break
                fi
            done
        done
    fi

    lengthOfFirstName=$((3 + $RANDOM % 12))
    lengthOfLastName=$((3 + $RANDOM % 12))
    indexCountry=$((0 + $RANDOM % $lengthOfCountries))

    age=$((1 + $RANDOM % 120))
    firstName=$(head -3 /dev/urandom | tr -cd '[:alpha:]' | cut -c -$lengthOfFirstName)
    lastName=$(head -3 /dev/urandom | tr -cd '[:alpha:]' | cut -c -$lengthOfLastName)
    
    indexVirus=$((0 + $RANDOM % $lengthOfViruses))

    date="$((1 + RANDOM % 30))-$((1 + RANDOM % 12))-$((1950 + RANDOM % 70))"

    if [ $(($RANDOM % 2)) -eq 1 ]
    then
        status='YES'
        echo $id $firstName $lastName ${arrCountries[indexCountry]} $age ${arrViruses[indexVirus]} $status  $date
    else
        status='NO'
        echo $id $firstName $lastName ${arrCountries[indexCountry]} $age ${arrViruses[indexVirus]} $status
    fi

    if [ "${arrayAlreadyCreated[$id]}" -eq "" ]
    then
        arrayAlreadyCreated[$id]="$id $firstName $lastName ${arrCountries[indexCountry]} $age"
    fi
done