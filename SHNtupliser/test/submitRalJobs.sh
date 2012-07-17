#!/usr/bin/env bash

pattern=$1
tag=$2
mcOrData=$3
safety=$4


outputDir=/opt/ppd/scratch/harper/Photon_PromptV4_run165633-165969/${tag}
if [ ! -d $outputDir ]; then
echo making dir $outputDir
mkdir -p $outputDir
fi


qsubLogDir=/opt/ppd/scratch/harper/qsubLogs/

maxEvents=-1


ntupConfigFile=shNtupliser_cfg.py


logDir=logs/${tag}
if [ ! -d $logDir ]; then
echo making dir $logDir
mkdir -p $logDir
fi

for file in `cat $pattern`
do

outputFile=`echo $file | awk -F "/" '{print $NF}' | sed 's/.root/_shNtup.root/g'`
logFile=`echo $file | awk -F "/" '{print $NF}' | sed 's/.root/_shNtup.log/g'`

fileNr=`echo $file | awk -F "/" '{print $NF}' | awk -F "_" '{print $5}'`
#outputFile=rateTest_${tag}_${fileNr}.root
#logFile=rateTest_${tag}_${fileNr}.log


echo $file $outputFile $logFile
if [[ "$safety" == "OFF" ]]; then
cat ntuplise_base.sh | \

sed 's|NTUPCONFIGFILE|'$ntupConfigFile'|g' | \
sed 's|INPUTFILE|'$file'|g' | \
sed 's|OUTPUTFILE|'$outputDir/$outputFile'|g' |  \
sed 's|LOGFILE|'$logDir/$logFile'|g' > simpleQSub_autoGen.sh
qsub simpleQSub_autoGen.sh -j oe -q prod -o $qsubLogDir -l mem=1500mb -l walltime=08:00:00
else 
echo safety is on
fi
done
