#!/usr/bin/env bash

#small script to give crab a command line interface

datasetPath=$1
nrEvents=$2
nrJobs=$3
outputFile=$4
outputPath=$5
datasetCode=$6
sampleWeight=$7
baseCfg=$8
workingDir=$9

#dbsUrl="dbs_url=http://cmsdbsprod.cern.ch/cms_dbs_ph_analysis_03/servlet/DBSServlet"
#dbsUrl="dbs_url=phys03"

scheduler=glitecoll
#workingDir=`echo $datasetPath | awk -F "/" '{print $2"_"$3}' `.`date +%y%m%d`_`date +%H%M%S`
copyData=0
returnData=1

#nrEvents=10 
#nrJobs=2

hltName=`echo $datasetPath | awk -F "S09" '{if(NF>1) print "REDIGI";else print "HLT"}'` 
hltName=HLT

echo here

echo $datsetPath $nrEvents $nrJobs $workingDir

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
sed 's|DBS_URL|'$dbsUrl'|' > crab_autoGen.cfg

sed 's|OUTPUTFILE|'$outputFile'|' $baseCfg | \
sed 's|SAMPLEWEIGHT|'$sampleWeight'|' | \
sed 's|isCrabJob=False|isCrabJob=True|' | \
sed 's|#CRABHLTNAMEOVERWRITE|hltName=\"'$hltName'\"|' | \
sed 's|DATASETCODE|'$datasetCode'|' > shNtupliser_autoGen_cfg.py


crab -create -cfg crab_autoGen.cfg
#crab -c $workingDir -submit

 
