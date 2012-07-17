#!/usr/bin/env bash

pattern=$1

for dir in `ls -d $pattern`
do
crab -c $dir -status
crab -c $dir -get

done