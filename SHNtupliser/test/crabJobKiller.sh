#!/usr/bin/env bash

dirPattern=$1
safety=$2

if [[ "$safety" == "OFF" ]] ; then
echo "about to kill crab jobs for real, you have 10seconds to abort..." 
sleep 10s
fi

for dir in `ls $dirPattern -d `
do


if [[ "$safety" == "OFF" ]] ; then

crab -c $dir -kill all

else
echo crab -c $dir -kill all

fi

done

