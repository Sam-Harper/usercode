#!/usr/bin/env bash

dirPattern=$1

for dir in `ls $dirPattern -d`
do
#echo $dir

if [ -e $dir/share/arguments.xml ]; then
#echo "$dir has no jobs" 

nrJobs=`cat $dir/share/arguments.xml | grep JobID | wc -l`

ralDir=/pnfs/pp.rl.ac.uk/data/cms/store/user/sharper

dataset=`echo $dir | awk -F "." '{print $1}' | awk -F "_DATA_" '{print $2}'`
if [ -z $dataset ];then
dataset=`echo $dir | awk -F "." '{print $1}' | awk -F "_MC_" '{print $2}'`
tag=`echo $dir | awk  -F "." '{print $2}' | awk -F "_${dataset}_" '{print $2}' | awk -F "_AODSIM" '{print $1}'`
samsVersion=`echo $dir | awk  -F "." '{print $2}' | awk -F "_${dataset}_" '{print $1}' | sed 's|_|/|'`
subDir=$samsVersion/$dataset/$tag/AODSIM

#echo tag $tag
#echo dataset $dataset
#echo ver $samsVersion

else 
#subDir=`echo $dir | awk  -F "." '{print $2}' | awk -F "${dataset}_" '{print $1,$2}' | awk -v dataset="$dataset" -F "_" '{print $1"/"$2"/"dataset"/"$3"/"$4}'`
subDir=`echo $dir | awk  -F "." '{print $2}' | sed 's|_|/|g'`
fi


echo ./missingFileFinder.sh $ralDir/$subDir $nrJobs

fi

done

