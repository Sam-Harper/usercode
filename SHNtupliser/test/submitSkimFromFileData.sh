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
nrJobs=10
datasetCode=0
outputFile=${dataset}_ntuples_${cmsswVersion}.root



#minRun=146428
#maxRun=146644

#minRun=147196
#maxRun=148058



#the output directory /pnfs/pp.rl.ac.uk/data/cms/store/user/harper/$outputPath



#outputPath=383/v17/EleSkimV2/${dataset}/Run${minRun}-${maxRun}
phoSkim=True
outputPath=423/triggerSkims/${dataset}/HT250/Run${minRun}-${maxRun}

#outputPath=383/RAW-RECO/Ele12SWTightEleIdIsolSkim/$dataset/Run${minRun}-${maxRun}
jsonFile=run${minRun}-${maxRun}_JSON.txt

rm -f $jsonFile

if [ "$minRun" -le "144114" ]; then
masterJson=Cert_132440-144114_7TeV_Sep17ReReco_Collisions10_JSON.txt 

else
#masterJson=Cert_132440-149442_7TeV_StreamExpress_Collisions10_JSON_v2.txt
#masterJson=Cert_136033-149442_7TeV_Nov4ReReco_Collisions10_JSON.txt 
masterJson=json_DCSONLY_ManualCert.txt 
fi
echo $minRun $maxRun, master Json: $masterJson

./fuckIt.py $minRun $maxRun $masterJson  $jsonFile >& temp.log
baseCfg="hltSkim.py"


subDirs=`echo $outputPath | sed 's|/|_|g'`
workingDir=`echo $datasetPath | awk -F "/" '{print $2"_"$3}' `.${subDirs}.`date +%y%m%d`_`date +%H%M%S`

if [[ "$live" == "ON" ]] ; then
echo about to submit for real ./submitCrabJobData.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $datasetCode $runningOn35X $baseCfg $jsonFile $workingDir  $phoSkim
sleep 5s
 ./submitCrabJobData.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $datasetCode $runningOn35X  $baseCfg $jsonFile $workingDir $phoSkim $minRun $maxRun
#sleep 30m
else
echo ./submitCrabJobData.sh $datasetPath $nrEvents $nrJobs $outputFile $outputPath $datasetCode $runningOn35X $baseCfg $jsonFile $workingDir  $phoSkim $minRun $maxRun


fi

done <  tempDatasets
