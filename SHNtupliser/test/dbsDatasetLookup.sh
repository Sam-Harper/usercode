#!/usr/bin/env bash

datasets=$1

for dataset in `cat $datasets`
do
nrEvents=`dbs search --query="find sum(block.numevents) where dataset like $dataset"`
nrEvents=`echo $nrEvents | awk '{print $NF}'`
nrFiles=`dbs search --query="find count(file) where dataset like $dataset"` 
nrFiles=`echo $nrFiles | awk '{print $NF}'`
#crossSec=`dbs search --query="find dataset.xsection where dataset like $dataset" | awk '{print $NF}'`
#echo $crossSec
#line=`echo $data | awk -F "sum.block." '{print $2}' | awk '{print $3" & "$4" & 1.0 & 1.0 & 0 & 100"}'`
echo "$dataset & $nrEvents & $nrFiles & 1.0 & 1.0 & 0 & 100"
done
