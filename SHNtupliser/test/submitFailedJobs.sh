#!/usr/bin/env bash


jobData=$1
jobs=$2

jobs=`echo $jobs | sed 's/,/ /g'`

outBaseDir=/media/disk/data/

crabDir=`echo $jobData | awk -F "/" '{print $(NF-2)}'`
dataset=`echo $crabDir | awk -F "." '{print $1}' | awk -F "crabJob_MC_" '{print $2}'`
id=`echo $crabDir | awk -F "." '{print $2}' | awk -F "_" '{print $3}'`
id=${id}_`echo $crabDir | awk -F "." '{print $2}' | awk -F "_" '{print $2}'`
id="L1Ntuples"

outDir=`echo $jobData | awk -F "/" '{print $(NF-2)}' | awk -F "." '{print $2}'`
logDir=logs/$outDir
logDir=logs/$dataset
outDir=$outBaseDir$outDir
outDir=$outBaseDir/$dataset
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
#echo lumis: $inputLumis
#echo files: $inputFiles



cat shL1Ntupliser_mc.py > shNtupliser_autoGen.py

echo "process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange(" >> shNtupliser_autoGen.py
echo $inputLumis | awk -F "," '{ for(i=1;i<=NF;i++) print "\""$i"\","} ' >>shNtupliser_autoGen.py
echo ")" >> shNtupliser_autoGen.py



echo "process.source.fileNames = cms.untracked.vstring(" >> shNtupliser_autoGen.py
echo $inputFiles | awk -F "," '{ for(i=1;i<=NF;i++) print "\""$i"\","} ' >>shNtupliser_autoGen.py
echo ")" >> shNtupliser_autoGen.py


#outputFile=DoublePhotonHighPt_ntuples_SHv24B_${runRange}_${job}_ral
outputFile=${dataset}_${id}_${job}

#echo $outputFile
echo "cmsRun shNtupliser_autoGen.py dummy ${outDir}/${outputFile}.root $logDir/${outputFile}.log "
cmsRun shNtupliser_autoGen.py dummy ${outDir}/${outputFile}.root >& $logDir/${outputFile}.log 
#sleep 20s



done
