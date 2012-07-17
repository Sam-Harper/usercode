#!/usr/bin/env bash


source /gridsoft/SL5/cms/cmsset_default.sh
export SCRAM_ARCH=slc5_amd64_gcc434


cd /opt/ppd/scratch/harper/CMSSW_423_SHNtup/src/SHarper/JobSubmission/test

outFile=/opt/ppd/scratch/harper/Photon_PromptV4_run165633-165969/pho_SHv19/3E00574B-D58A-E011-ADC8-003048F117EC_shNtup.root

echo current dir $PWD
echo tempDir $TMPDIR
eval `scramv1 runtime -sh`
ls $TMPDIR



echo "starting NTUP Job ">>logs/pho_SHv19/3E00574B-D58A-E011-ADC8-003048F117EC_shNtup.log
cmsRun shNtupliser_cfg.py /store/data/Run2011A/Photon/AOD/PromptReco-v4/000/165/633/3E00574B-D58A-E011-ADC8-003048F117EC.root  $TMPDIR/ntup.root >> logs/pho_SHv19/3E00574B-D58A-E011-ADC8-003048F117EC_shNtup.log 2>&1


cp $TMPDIR/ntup.root $outFile