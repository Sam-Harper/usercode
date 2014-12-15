#!/usr/bin/env bash

pattern=$1
trigger=$2
for dir in $pattern
do
echo -n $dir
cat $dir/res/*.stdout  | grep $trigger | grep -v Time | grep -v Modules | awk '{nrTot+=$4;nrPass+=$5}END{print " nrTot ",nrTot," nrPass", nrPass}'
done
  
