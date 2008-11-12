#!/usr/bin/env bash

pattern=$1
safety=$2

for crabJob in `ls $pattern* -d`
do

if [[ "$safety" == "OFF" ]] ; then
echo about to crab -c $crabJob -kill all
crab -c $crabJob -kill all
rm $crabJob
else
echo would crab -c $crabJob -kill all

fi

done
