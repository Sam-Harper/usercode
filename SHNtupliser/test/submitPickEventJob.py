#!/usr/bin/env python

import os

import argparse

parser = argparse.ArgumentParser(description='makes the json & event list files for pickEvents')

parser.add_argument('--dataset',required=True,help='dataset')
parser.add_argument('--input',required=True,help='input event list')
parser.add_argument('--pub',action='store_true',help='publish or not')
parser.add_argument('--dryRun',action='store_true',help='if set, do not actually submit anything')
parser.add_argument('--ignoreLocality',action='store_true',help="whether to ignore locality")
parser.add_argument('--unitsPerJob',required=True,help='number of units per job')
parser.add_argument('--majorVersion',required=True,help='top level version')
parser.add_argument('--minorVersion',required=True,help='2nd level version')

args = parser.parse_args()

import subprocess
subprocess.Popen(['edmPickEvents.py',args.dataset,args.input,"--crab"]).communicate()

event_list="pickevents_runEvents.txt"
lumi_mask="pickevents.json"


dataset=args.dataset.split("/")[1]
datasetId = args.dataset.split("/")[2]
#datasetId = datasetId[datasetId.find("-")+1:]
datasetFormat = args.dataset.split("/")[-1]

outputFile=dataset+"_"+datasetId+"_"+args.minorVersion+".root"
outputPath="/store/user/sharper/"+args.majorVersion+"/"+args.minorVersion+"/"+datasetFormat
publishDataname=datasetId+"_"+datasetFormat+"_"+args.minorVersion
# /store/user/<dir>[/<subdirs>]/<primary-dataset>/<publication-name>/<time-stamp>/<counter>/<file-name>
import datetime
currTime=datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
workingDir="PickEvts_"+dataset+"_"+args.majorVersion+"_"+args.minorVersion+"_"+datasetFormat+"_"+datasetId+"_"+currTime

crab_base_file="crab_pickEvents.py"
with open(crab_base_file,"r") as f:
    configLines = f.readlines()

crab_file="crab_autogen.py"
with open(crab_file,"w") as f:
    for line in configLines:
        line = line.replace("#TOSED:EXTRAJOBTYPEINFO","config.JobType.pyCfgParams = ['eventsToProcess_load=%s', 'outputFile=%s']"% (event_list,outputFile))
        f.write(line)

    
crabSubmitCmd = "crab submit -c "+crab_file+" General.requestName="+workingDir+ \
    " Data.inputDataset="+args.dataset+ \
    " Data.unitsPerJob="+str(args.unitsPerJob)+ \
    " Data.outLFNDirBase="+outputPath+ \
    " Data.ignoreLocality="+str(args.ignoreLocality)+ \
    " Data.outputDatasetTag="+publishDataname+ \
    " Data.lumiMask="+lumi_mask + \
    " Data.publication="+str(args.pub)
print "will submit:"
print crabSubmitCmd
print " "

if not args.dryRun:
    print "submitting for REAL"
    import time
    time.sleep(5)
    import os

    os.system(crabSubmitCmd)

print "All done"
