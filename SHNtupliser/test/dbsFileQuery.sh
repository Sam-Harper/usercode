#!/usr/bin/env bash

run=$1
lumi=$2

file=`dbs search --query="find file where dataset like /Photon/Run2011A*/AOD and lumi>=$2 and lumi<=$3 and run=$1" | grep Photon`

echo $file

 #awk '{if($0 in stored_lines) x=1;else {stored_lines[$0]=1;print $0}}'
