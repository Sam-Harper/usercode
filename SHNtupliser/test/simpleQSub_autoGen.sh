#!/usr/bin/env bash


export SCRAM_ARCH=slc5_amd64_gcc462
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch/
source $VO_CMS_SW_DIR/cmsset_default.sh

cd /home/ppd/mjf21517/scratch/CMSSW/CMSSW_534_SHNtup/src/SHarper/SHNtupliser/test

outFile=/opt/ppd/scratch/harper/data//Photon_Run2012A-13Jul/534/PreShower//Photon_Run2012A-13Jul_003D1FBB-66D0-E111-B5E2-00304866C368_ntuples_SHv24B_deLasered.root

echo current dir $PWD
echo tempDir $TMPDIR
eval `scramv1 runtime -sh`
ls $TMPDIR

echo "starting ntup Job ">/opt/ppd/scratch/harper/data/logs//Photon_Run2012A-13Jul/534/PreShower//Photon_Run2012A-13Jul_003D1FBB-66D0-E111-B5E2-00304866C368_ntuples_SHv24B_deLasered.log

cmsRun ntupReRunAutoGen.py /store/data/Run2012A/Photon/AOD/13Jul2012-v1/00000/003D1FBB-66D0-E111-B5E2-00304866C368.root $TMPDIR/ntup.root >> /opt/ppd/scratch/harper/data/logs//Photon_Run2012A-13Jul/534/PreShower//Photon_Run2012A-13Jul_003D1FBB-66D0-E111-B5E2-00304866C368_ntuples_SHv24B_deLasered.log 2>&1


cp $TMPDIR/ntup.root $outFile