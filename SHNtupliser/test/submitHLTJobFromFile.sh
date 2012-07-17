#!/usr/bin/env bash

#nrEventsPerJob=10000 #dont think this does anything

datasetFile=$1
datasetPattern=$2
live=$3
datasetAntiPattern=$4
#for entry in `grep $dataset $datasetFile | awk -F "&" '{print $1}'`
grep $datasetPattern $datasetFile | grep -v $datasetAntiPattern > tempDatasets



#for entry in `grep $datasetPattern $datasetFile | grep -v $datasetAntiPattern | awk -F "&" '{print $1}' `
while read line
do
#line=`grep $entry $datasetFile`
#line=$entry
#echo $line

datasetPath=`echo $line | awk -F "&" '{print $1}'`
dataset=`echo $datasetPath | awk -F "/" '{print $2}'`
nrEvents=`echo $line | awk -F "&" '{print $2}' | awk '{print $1}'`
nrJobs=`echo $line | awk -F "&" '{print $3}' | awk '{print $1}'`
cmsswVersion=`echo $datasetPath | awk -F "/" '{print $3}'`


outputFile=${dataset}_${cmsswVersion}_TrigReRun.root



outputPath=428/trigWork/RAWMCSkims/${cmsswVersion}/${dataset}



baseCfg="hltSkim.py"



subDirs=`echo $outputPath | sed 's|/|_|g'`
workingDir=`echo $datasetPath | awk -F "/" '{print "crabJob_"$2}' `.${subDirs}.`date +%y%m%d`_`date +%H%M%S`



if [[ "$live" == "ON" ]] ; then
echo about to submit for real ./submitCrabHLT.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath  $baseCfg  $workingDir  
sleep 5s
 ./submitCrabJobHLT.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $baseCfg  $workingDir 
#sleep 60m
else
echo  ./submitCrabJobHLT.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $baseCfg  $workingDir 


fi

done <  tempDatasets
