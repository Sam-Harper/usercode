#!/usr/bin/env bash


triggerList=$1

may10ReRecoCert=$2
#promptRecoCert=$3

for trigger in `cat $triggerList`
do

echo "lumi of trigger $trigger"
lumiCalc.py -i $may10ReRecoCert overview -hltpath $trigger

#echo "lumi in prompt"
#lumiCalc.py -i $promptRecoCert overview -hltpath $trigger

done

echo "all done"