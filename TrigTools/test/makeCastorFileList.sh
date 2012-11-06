#!/usr/bin/env bash

dir=$1

files=`rfdir $dir | awk '{print $9}'`
dirWOCastor=`echo $dir | awk -F /store/ '{print "/store/"$2}'`
for file in $files
do 

#dirWOCastor=`echo $dir | awk -F /store/ '{print "/store/"$2}' 

echo \'$dirWOCastor/$file\',

done
