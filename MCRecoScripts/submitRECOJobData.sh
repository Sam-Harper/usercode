#!/usr/bin/env bash

#small script to automatically submit MC RECO jobs for EXO

datasetPath=$1
nrEvents=$2
nrEventsPerJob=$3
#outputStorageElement=T2_UK_SGrid_RALPP
outputStorageElement=$4 #the T2 where you want this sample
version=$5  #a user specified version tag incase you need to re-run, defaults to v1

if [ -z "$version" ];then
    version=v1
fi

#these are the identifiers of the conditions and campaign
config=reco_700_data.py

dbsUrl="dbs_url=phys03"


reRECOVersion=`echo $CMSSW_VERSION | sed 's/CMSSW_//g' | sed 's/_//g'`

runEraAndSkim=`echo $datasetPath | awk -F "/" '{print $3}' | awk -F "-" '{print $2}'`
globalTag=`python $config input.root output.root | grep "globaltag" | awk '{print $3}' | awk -F ":" '{print $1}'`

conditions=$globalTag
publishDataname=${runEraAndSkim}_${reRECOVersion}_${conditions}-${version}_RECO
dbsUrlForPub="https://cmsdbsprod.cern.ch:8443/cms_dbs_ph_analysis_02_writer/servlet/DBSServlet"

echo $conditions

#allows us to publish the data and copy to T2
publishData=1
copyData=1
returnData=0

datasetName=`echo $datasetPath | awk -F "/" '{print $2}'`
outputPath=data/$reRECOVersion/$datasetName/$conditions/${version}  #note, outputPath is overriden by crab when publishing, this is only when we are publishData=0
outputFile=${datasetName}_${reRECOVersion}_${conditions}-${version}.root


#subDirs=`echo $outputPath | sed 's|/|_|g'`
subDirs=$publishDataname
workingDir=`echo $datasetPath | awk -F "/" '{print "crabJob_MC_"$2}' `.${subDirs}.`date +%y%m%d`_`date +%H%M%S`
echo "$datasetPath $nrEvents $nrEventsPerJob $outputPath $outputFile $workingDir"

#we use sed to edit our output files prior to submision
#the sed order matters...
sed 's|TOSED:DATASETPATH|'$datasetPath'|' crab_base_reco.cfg | \
sed 's|TOSED:LUMISPERJOB|'$nrLumisPerJob'|' | \
sed 's|TOSED:TOTLUMIS|'$totLumis'|' | \
sed 's|TOSED:NREVENTS|'$nrEvents'|' | \
sed 's|TOSED:OUTPUTFILE|'$outputFile'|' | \
sed 's|TOSED:DBSURL|'$dbsUrl'|' | \
sed 's|TOSED:OUTPUTPATH|'$outputPath'|' | \
sed 's|TOSED:STORAGEELEMENT|'$outputStorageElement'|' | \
sed 's|TOSED:WORKINGDIR|'$workingDir'|' | \
sed 's|TOSED:RETURNDATA|'$returnData'|' | \
sed 's|TOSED:PUBLISHDATANAME|'$publishDataname'|' | \
sed 's|TOSED:PUBLISHDATA|'$publishData'|' | \
sed 's|TOSED:DBSURLFORPUB|'$dbsUrlForPub'|' | \
sed 's|TOSED:COPYDATA|'$copyData'|' > crab_autoGen.cfg

sed 's|TOSED:OUTPUTFILE|'$outputFile'|' $config | \
sed 's|isCrabJob=False|isCrabJob=True|'   > cmssw_autoGen.py



crab -create -cfg crab_autoGen.cfg
crab -c $workingDir -submit

 
