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
nrJobs=99999
datasetCode=0
outputFile=${dataset}_ntuples_${cmsswVersion}_SHv24B.root





#the output directory /pnfs/pp.rl.ac.uk/data/cms/store/user/harper/$outputPath

phoSkim=False

if [ "$dataset" == "Photon" ]; then
phoSkim=True
fi

echo phoSkim $phoSkim

outputPath=534/v24B/${cmsswVersion}/${dataset}/Run${minRun}-${maxRun}


jsonFile=run${minRun}-${maxRun}_JSON.txt

rm -f $jsonFile
#if [ ! -e $jsonFile ]; then
#./fuckIt.py $minRun $maxRun Cert_132440-149442_7TeV_StreamExpress_Collisions10_JSON.txt  $jsonFile
#./fuckIt.py $minRun $maxRun jsonls.txt  $jsonFile
#fi


if [ "$minRun" -le "144114" ]; then
masterJson=Cert_132440-144114_7TeV_Sep17ReReco_Collisions10_JSON.txt 

else
masterJson=json_DCSONLY_ManualCert.txt 
fi
echo $minRun $maxRun, master Json: $masterJson

./fuckIt.py $minRun $maxRun $masterJson  $jsonFile >& temp.log
#baseCfg="shNtupliser_withPUCorrJets_cfg.py"
#baseCfg="jetFilterSkim.py"
baseCfg="shNtupliser_data.py"


subDirs=`echo $outputPath | sed 's|/|_|g'`
workingDir=`echo $datasetPath | awk -F "/" '{print "crabJob_DATA_"$2}' `.${subDirs}.`date +%y%m%d`_`date +%H%M%S`


if [[ "$live" == "ON" ]] ; then
echo about to submit for real ./submitCrabJobData.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $datasetCode $runningOn35X $baseCfg $jsonFile $workingDir  $dataset $minRun $maxRun
sleep 5s
 ./submitCrabJobData.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $datasetCode $runningOn35X  $baseCfg $jsonFile $workingDir $dataset $minRun $maxRun
#sleep 60m
else
echo ./submitCrabJobData.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $datasetCode $runningOn35X $baseCfg $jsonFile $workingDir  $dataset $minRun $maxRun


fi

done <  tempDatasets
