#!/usr/bin/env bash

while read line; do

run=`echo $line | awk '{print $1}'`
lumi1=`echo $line | awk '{print $2}'`
lumi2=`echo $line | awk '{print $3}'`

 ./dbsFileQuery.sh $run $lumi1 $lumi2


done < $1
