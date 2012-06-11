#!/usr/bin/env bash


jobData=$1
jobs=$2

jobs=`echo $jobs | sed 's/,/ /g'`

outBaseDir=/opt/ppd/newscratch/harper/failedJobsRunManually/




outDir=`echo $jobData | awk -F "/" '{print $(NF-2)}' | awk -F "." '{print $2}'`
logDir=logs/$outDir
outDir=$outBaseDir$outDir
echo $outDir
mkdir -p $outDir
mkdir -p $logDir
runRange=`echo $jobData | awk -F "_" '{print $6}' | awk -F "." '{print $1}'`

for job in $jobs
do




inputLumiFiles=`xsltproc crabArgumentStyleSheet.xsl  $jobData  | awk -v jobId=$job -F "::" '{if(jobId==$1) print $1"::"$2"::"$3}'`
#echo $inputLumiFiles
inputFiles=`echo $inputLumiFiles | awk -F "::" '{print $3}'`
inputLumis=`echo $inputLumiFiles | awk -F "::" '{print $2}'`
echo lumis: $inputLumis
echo files: $inputFiles



cat shNtupliser_data.py > shNtupliser_autoGen.py

echo "process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange(" >> shNtupliser_autoGen.py
echo $inputLumis | awk -F "," '{ for(i=1;i<=NF;i++) print "\""$i"\","} ' >>shNtupliser_autoGen.py
echo ")" >> shNtupliser_autoGen.py



echo "process.source.fileNames = cms.untracked.vstring(" >> shNtupliser_autoGen.py
echo $inputFiles | awk -F "," '{ for(i=1;i<=NF;i++) print "\""$i"\","} ' >>shNtupliser_autoGen.py
echo ")" >> shNtupliser_autoGen.py


outputFile=Photon_ntuples_SHv20_${runRange}_${job}_ral
echo $outputFile
nice cmsRun shNtupliser_autoGen.py $inputFile ${outDir}/${outputFile}.root >& $logDir/${outputFile}.log &
sleep 20s



done
