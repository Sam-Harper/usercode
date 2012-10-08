#!/usr/bin/env bash



for dir in `ls $1 -d`
do

echo $dir
crab -c $dir -status | grep "Total Jobs"
crab -c $dir -get
crab -c $dir -status

done
