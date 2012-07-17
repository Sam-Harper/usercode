#!/usr/bin/env bash

for fileDir in `cat phoFilesTemp.list`
do 
    file=`echo $fileDir | awk -F "/" '{print $NF}'`

 cmsRun trigFilter_cfg.py $fileDir /media/disk-1/Run147219Pho17SC17Skim/${file}_Pho17SC17Skim.root

done
