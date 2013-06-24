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

echo $line

datasetPath=`echo $line | awk -F "&" '{print $1}'`
dataset=`echo $datasetPath | awk -F "/" '{print $2}'`
nrEvents=999999999
runningOn35X=False
cmsswVersion=`echo $datasetPath | awk -F "/" '{print $3}'`
minRun=`echo $line | awk -F "&" '{print $2}' | awk '{print $1}'`
maxRun=`echo $line | awk -F "&" '{print $3}' | awk '{print $1}'`
jsonFile=`echo $line | awk -F "&" '{print $4}'`
nrJobs=99999
datasetCode=0
outputFile=${dataset}_ntuples_${cmsswVersion}_SHL1v1.root





#the output directory /pnfs/pp.rl.ac.uk/data/cms/store/user/harper/$outputPath

phoSkim=False

if [ "$dataset" == "Photon" ]; then
phoSkim=True
fi

echo phoSkim $phoSkim

outputPath=601p4/SHL1v1/NoTowerThres/${cmsswVersion}/${dataset}/Run${minRun}-${maxRun}


#baseCfg="shNtupliser_withPUCorrJets_cfg.py"
#baseCfg="jetFilterSkim.py"
baseCfg="shL1Ntupliser_data.py"


subDirs=`echo $outputPath | sed 's|/|_|g'`
workingDir=`echo $datasetPath | awk -F "/" '{print "crabJob_DATA_"$2}' `.${subDirs}.`date +%y%m%d`_`date +%H%M%S`


if [[ "$live" == "ON" ]] ; then
echo about to submit for real ./submitCrabJobData.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $datasetCode $runningOn35X $baseCfg $jsonFile $workingDir  $dataset $minRun $maxRun
sleep 5s
 ./submitCrabJobDataJson.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $datasetCode $runningOn35X  $baseCfg $jsonFile $workingDir $dataset $minRun $maxRun
#sleep 60m
else
echo ./submitCrabJobDataJson.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $datasetCode $runningOn35X $baseCfg $jsonFile $workingDir  $dataset $minRun $maxRun


fi

done <  tempDatasets
