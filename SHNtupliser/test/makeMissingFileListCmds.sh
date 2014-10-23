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
tag=`echo $dir | awk  -F "." '{print $2}' | awk -F "_${dataset}" '{print $1}' | awk -F "_AODSIM_" '{print $2}'`
samsVersion=`echo $dir | awk  -F "." '{print $2}' | awk -F "_AODSIM_" '{print $1}' | sed 's|_|/|' | sed 's|_USER_|/USER/|g'`
subDir=$samsVersion/$tag/$dataset

#echo tag $tag
#echo dataset $dataset
#echo ver $samsVersion
else 
subDir=`echo $dir | awk  -F "." '{print $2}' | awk -F "$dataset" '{print $1,$2}' | awk -v dataset="$dataset" -F "_" '{print $1"/"$2"/"$3"/"dataset"/"$5}'`
fi


echo ./missingFileFinder.sh $ralDir/$subDir $nrJobs $dir

fi

done

