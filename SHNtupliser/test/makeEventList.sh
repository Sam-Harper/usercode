#!/usr/bin/env bash

baseConfig=eventPicker_base_cfg.py
jobNr=$2
eventList=$3

inFile="../python/zeeFiles_${jobNr}.py"

file=autoGen_${jobNr}_cfg.py

sed 's|JOBNR|'$jobNr'|g' $baseConfig > $file



echo "process.source.eventsToProcess = cms.untracked.VEventRange(" >> $file
for runnr in `cat $inFile | grep ".root" | awk -F "/" '{print $9$10}' | uniq`
do

echo $runnr

cat $eventList | awk '{if('$runnr'==$1) print "\""$1":"$2"\","}' >>$file

done

echo ")" >>$file
