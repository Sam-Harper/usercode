#!/usr/bin/env bash

#this is the script which crab jobs run
#necessary as HLT and RECO steps need to be seperate
#pset.py (aka step1) makes RAW + HLT
#step2.py makes RECO

jobNr=$1

echo running step 1
cmsRun -j $RUNTIME_AREA/crab_fjr_$jobNr.xml -p pset.py
echo running step 2
cmsRun -j $RUNTIME_AREA/crab_fjr_$jobNr.xml -p exoMCRERECO_step2_autoGen.py


