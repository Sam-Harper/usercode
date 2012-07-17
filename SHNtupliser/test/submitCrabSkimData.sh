#!/usr/bin/env bash

#small script to give crab a command line interface

datasetPath=$1
nrEvents=$2
nrJobs=$3
outputFile=$4
outputPath=$5
datasetCode=$6
runningOn35XSample=$7
baseCfg=$8
#dbsUrl=$9
jsonFile=${9}
workingDir=${10}
phoSkim=${11}
lowRun=${12}
highRun=${13}

lumisPerJob=50
#lumisPerJob=50
#number_of_jobs
totLumis=99999999

scheduler=glitecoll

#echo $workingDir2

copyData=1
returnData=0



echo $workingDir

echo $baseCfg

sed 's|DATASETPATH|'$datasetPath'|' crab_base_data.cfg | \

sed 's|NREVENTS|'$nrEvents'|' | \
sed 's|TOTLUMIS|'$totLumis'|' | \
sed 's|LUMISPERJOB|'$lumisPerJob'|' | \
sed 's|NRJOBS|'$nrJobs'|' | \
sed 's|LOWRUN|'$lowRun'|' | \
sed 's|HIGHRUN|'$highRun'|' | \
sed 's|OUTPUTFILE|'$outputFile'|' | \
sed 's|OUTPUTPATH|'$outputPath'|' | \
sed 's|WORKINGDIR|'$workingDir'|' | \
sed 's|SCHEDULER|'$scheduler'|' | \
sed 's|RETURNDATA|'$returnData'|' | \
sed 's|COPYDATA|'$copyData'|' | \
sed 's|DBS_URL|'$dbsUrl'|' | \
sed 's|JSONFILE|'$jsonFile'|' > crab_autoGen.cfg

sed 's|dummy.root|'$outputFile'|' $baseCfg | \
sed 's|SAMPLEWEIGHT|'$sampleWeight'|' | \
sed 's|isCrabJob=False|isCrabJob=True|' | \
sed 's|#PHOSKIMOVERWRITE|phoSkim='$phoSkim'|' | \
sed 's|#CRAB35XSAMPLEOVERWRITE|runningOn35XSample='$runningOn35XSample'|' | \
sed 's|DATASETCODE|'$datasetCode'|' > shNtupliser_autoGen_cfg.py


crab -create -cfg crab_autoGen.cfg
crab -c $workingDir -submit

 
