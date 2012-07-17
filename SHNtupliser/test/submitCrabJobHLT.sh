#!/usr/bin/env bash

#small script to give crab a command line interface

datasetPath=$1
nrEvents=$2
nrJobs=$3
outputFile=$4
outputPath=$5
baseCfg=$6
workingDir=${7}


lumisPerJob=100
#lumisPerJob=50
totLumis=99999999

scheduler=glitecoll

#echo $workingDir2

copyData=1
returnData=0

#nrEvents=10
#nrJobs=2

#lumisPerJob=10
#totLumis=10

echo $workingDir

echo $baseCfg

sed 's|DATASETPATH|'$datasetPath'|' crab_base.cfg | \

sed 's|NREVENTS|'$nrEvents'|' | \
sed 's|NRJOBS|'$nrJobs'|' | \
sed 's|OUTPUTFILE|'$outputFile'|' | \
sed 's|OUTPUTPATH|'$outputPath'|' | \
sed 's|WORKINGDIR|'$workingDir'|' | \
sed 's|SCHEDULER|'$scheduler'|' | \
sed 's|RETURNDATA|'$returnData'|' | \
sed 's|COPYDATA|'$copyData'|' | \
sed 's|DBS_URL|'$dbsUrl'|' | \
sed 's|JSONFILE|'$jsonFile'|' > crab_autoGen.cfg

sed 's|OUTPUTFILE|'$outputFile'|' $baseCfg | \
sed 's|SAMPLEWEIGHT|'$sampleWeight'|' | \
sed 's|isCrabJob=False|isCrabJob=True|' | \
sed 's|DATASETCODE|'$datasetCode'|' > shNtupliser_autoGen_cfg.py


crab -create -cfg crab_autoGen.cfg
crab -c $workingDir -submit

 
