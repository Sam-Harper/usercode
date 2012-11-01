#!/usr/bin/env bash

inputFilePattern=$1

cmsRun HEEPAnalyzerBarePAT_cfg.py $inputFilePattern  heepVal_barePat.root
cmsRun HEEPAnalyzerHEEPPAT_cfg.py $inputFilePattern  heepVal_heepPat.root
cmsRun HEEPAnalyzerWithPat_cfg.py $inputFilePattern  heepVal_withPat.root
cmsRun HEEPAnalyzer_cfg.py $inputFilePattern heepVal_std.root
cmsRun HEEPIdInLayer1Pat_cfg.py $inputFilePattern heepVal_heepIdInPat.root
