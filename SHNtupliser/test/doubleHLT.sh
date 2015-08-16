#!/usr/bin/env bash

cmsRun -j FrameworkJobReportPre.xml -p PSet.py
ls
cmsRun -j FrameworkJobReport.xml -p cmssw_temp2.py

