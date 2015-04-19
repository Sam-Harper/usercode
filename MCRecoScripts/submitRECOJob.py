#!/usr/bin/env python

#small script to automatically submit MC RECO jobs for EXO
#yes this is a direct port of a bash script...

import os
import optparse
import sys

parser = optparse.OptionParser(description='submits RECO jobs to crab')
parser.add_option('--dataset',help='dataset name')
parser.add_option('--nrEvents',help='number of events to process')
parser.add_option('--nrEventsPerJob',help='number of events / job')
parser.add_option('--outputStorageElement',help='output storage element eg T2_UK_SGrid_RALPP')
parser.add_option('--copyData',help="whether to stage out the data or not",default=True)
parser.add_option('--dbsUrl',help="dbs url global,phys03 etc",default="global")
parser.add_option('--dryRun',help="if true, is a dry run, do not actually submit crab jobs",default=True)
options,args = parser.parse_args()
if not options.dataset or not options.nrEvents or not options.nrEventsPerJob or not options.outputStorageElement:
    parser.error("dataset,nrEvents, nrEventsPerJob and outputStorageElement are manditory")
    sys.exit(0)

cmsswVersion="740"

#these are the identifiers of the conditions and campaign
config="reco_740_POSTLS1"

datasetName=options.dataset.split("/")[1]
puIndex=2;
if datasetName.find("DYJetsToLL")==0:
    print "DYJets detected"
    config=config+"_DYToEEFilter_RECODEBUG.py"
elif datasetName.find("RSGravToGG")==0:
    print "RSGrav -> gamma gamma detected"
    config=config+"_FEVTSIM.py"
elif datasetName.find("QCD")==0:

    #print "QCD detected, running event filtered"
    #config=config"_EvtFilter.py"
    print "QCD detected"
    config=config+"_EGSkim.py"
    puIndex=3
elif datasetName.find("ZprimeToEE")==0:
    print "Z'->ee detected"
    puIndex=0
    config=config+".py"
else:
    print ("dataset %s not detected, aborting" %datasetName)
    sys.exit(0)



if config.find("_EvtFilter.py")!=-1:
    fullFilename="evtLists/"+datasetName+"_evts_trackCuts.list"
    if os.path.isfile(fullFilename):
        import shutil
	shutil.copy(fullFilename,"data/evtList.list") 
    else:
	print "no event list file "+fullFilename+" found, aborting "
    sys.exit(0)


pileUp=options.dataset.split("/")[2].split("_")[puIndex]
#timing=`python $config dummy dummy dummy | grep "3D Timing" | awk '{if(NF>=4) print "_"$4}'`
#reRECOVersion="EGM720${timing}_EGIsolMapV1_Ele20Skim"
reRECOVersion="EGM730"
datasetTIER="RECO"
#globalTag=`python $config input.root output.root | grep "globaltag" | awk '{print $3}' | awk -F ":" '{print $1}'`
globalTag="Default"
if pileUp.find("bx25")!=-1: 
    globalTag="PHY1474_25V2"
elif pileUp.find("bx50")!=-1:
    globalTag="NotReady"
else:
    print "pile up senario "+pileUp+", not recognised, cant determin global tag"
    sys.exit(0)

conditions=pileUp+"_"+globalTag
publishDataname=reRECOVersion+"_"+conditions+"_"+datasetTIER
dbsUrlForPub="https://cmsdbsprod.cern.ch:8443/cms_dbs_ph_analysis_02_writer/servlet/DBSServlet"

print "config: reRECOVersion:  conditions: ",config,reRECOVersion,conditions


outputFile=datasetName+"_"+reRECOVersion+"_"+conditions+"_"+datasetTIER+".root"
outputPath="/store/user/sharper/"+cmsswVersion+"/RERECO/"+reRECOVersion+"/"+datasetTIER
publishDataname=conditions


import datetime
currTime=datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
workingDir="MC_"+datasetName+"_"+cmsswVersion+"_RERECO_"+reRECOVersion+"_"+conditions+"_"+datasetTIER+"_"+currTime

nrEvents = options.nrEvents
nrEventsInLumiSec=100
totalUnits=-1
if options.nrEvents!=-1:
    totalUnits = int(options.nrEvents)/nrEventsInLumiSec 
unitsPerJob = int(options.nrEventsPerJob)/nrEventsInLumiSec

with open(config,"r") as configFile:
    configLines = configFile.readlines()
   
tempConfig="cmssw_autoGen.py"
with open(tempConfig,"w") as tempConfigFile:
    for line in configLines:
        line=line.replace("TOSED:GLOBALTAG",globalTag)
        line=line.replace("TOSED:OUTPUTFILE",outputFile)
        line=line.replace("isCrabJob=False","isCrabJob=True");
        tempConfigFile.write(line)


crabSubmitCmd = "crab submit -c crab_base.py General.requestName="+workingDir+ \
                " General.transferOutputs="+str(options.copyData)+ \
                " Data.inputDataset="+options.dataset+ \
                " Data.inputDBS="+options.dbsUrl+ \
                " Data.unitsPerJob="+str(unitsPerJob)+ \
                " Data.totalUnits="+str(totalUnits)+ \
                " Data.outLFN="+outputPath+ \
                " Data.publishDataName="+publishDataname+ \
                " Site.storageSite="+options.outputStorageElement+\
                " JobType.psetName="+tempConfig
   

print "will submit:"
print crabSubmitCmd
print " "

if options.dryRun=="False":
    print "submitting for REAL"
    import time
    time.sleep(5)
    import os
    os.system(crabSubmitCmd)
