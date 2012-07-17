#!/usr/bin/env bash

dir=$1
rfdir $dir | awk -v var1="$dir" '{print "\"rfio:"var1"/"$9"\","}'
#for file in `rfdir $dir | awk -v dir="$dir" '{print $dir"/"$9}'`
#do
#echo $file

#done

