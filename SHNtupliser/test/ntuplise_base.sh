#!/usr/bin/env bash


source /gridsoft/SL5/cms/cmsset_default.sh
export SCRAM_ARCH=slc5_amd64_gcc434


cd /opt/ppd/scratch/harper/CMSSW_423_SHNtup/src/SHarper/JobSubmission/test

outFile=OUTPUTFILE

echo current dir $PWD
echo tempDir $TMPDIR
eval `scramv1 runtime -sh`
ls $TMPDIR



echo "starting NTUP Job ">>LOGFILE
cmsRun NTUPCONFIGFILE INPUTFILE  $TMPDIR/ntup.root >> LOGFILE 2>&1


cp $TMPDIR/ntup.root $outFile