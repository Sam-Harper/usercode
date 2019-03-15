#!/usr/bin/env python

def getGlobalTag(configName):
    import imp
    old_sysargv = list(sys.argv)
    sys.argv=sys.argv[:1]
    module = imp.load_source("process",configName)
    sys.argv= list(old_sysargv)
    return module.process.GlobalTag.globaltag.value()

import os
import optparse
import sys

parser = optparse.OptionParser(description='submits gen jobs to crab')

parser.add_option('--dataset',help='dataset name')
parser.add_option('--dryRun',help="if true, is a dry run, do not actually submit crab jobs",default=True)
parser.add_option('--config',help='cmsRun config file to run')
parser.add_option('--outDir',help='cmssw version')
parser.add_option('--copyData',help="whether to stage out the data or not",default=True)
parser.add_option('--version',help="dataset version",default="v1")

parser.add_option('--dbsUrl',help="dbs url global,phys03 etc",default="global")
parser.add_option('--transferLogFiles',help="whether to transfer log files or not",default=False)
parser.add_option('--datasetTag',help='tag for publish datasetname')
parser.add_option('--nrJobs',help='number of jobs to run')
parser.add_option('--nrEventsPerJob',help='number of events to gen per job')
parser.add_option('--publish',help="publish or not")
options,args = parser.parse_args()
if not options.dataset or not options.datasetTag or not options.outDir or not options.config or not options.nrJobs or not options.nrEventsPerJob or not options.publish:
    parser.error("dataset, datasetTag, config, nrJobs, nrEventsPerJob, publish and outDir are manditory")
print options.config

#pub name = <datasetTag>_CMSSWVersion_GlobalTag-version (kinda follows official convention)
cmsswVersion=os.getenv("CMSSW_VERSION").replace("_","")
globalTag=getGlobalTag(options.config).split("::")[0].lower().replace("_","-")
pubName = options.datasetTag+"_"+cmsswVersion+"_"+globalTag+"_"+options.version
print pubName

crabProjDir='crab_projects'

datasetFormat="GEN-SIM"

outputFile=options.dataset+"_"+pubName+"_"+datasetFormat+".root"
outputPath="/store/user/sharper/"+options.outDir+"/"+datasetFormat # /store/user/<dir>[/<subdirs>]/<primary-dataset>/<publication-name>/<time-stamp>/<counter>/<file-name>
import datetime
currTime=datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
workingDir="MC_"+options.outDir.replace("/","-")+"_"+options.dataset+"_"+pubName+"_"+currTime
workingDirTmp="MC_"+options.dataset+currTime

with open(options.config,"r") as configFile:
    configLines = configFile.readlines()
   
    tempConfig="cmssw_temp.py"
    with open(tempConfig,"w") as tempConfigFile:
        for line in configLines:
            line=line.replace("OUTPUTFILE.root",outputFile)
            tempConfigFile.write(line)

  

crabSubmitCmd = "crab submit -c crab_gen.py --wait General.requestName="+workingDirTmp+ \
    " General.transferOutputs="+str(options.copyData)+ \
    " Data.outputPrimaryDataset="+options.dataset+ \
    " Data.unitsPerJob="+str(options.nrEventsPerJob)+ \
    " Data.totalUnits="+str(int(options.nrJobs)*int(options.nrEventsPerJob))+ \
    " Data.outLFNDirBase="+outputPath+ \
    " Data.outputDatasetTag="+pubName+ \
    " JobType.psetName="+tempConfig+ \
    " General.workArea="+crabProjDir+ \
    " General.transferLogs="+str(options.transferLogFiles) + \
    " Data.publication="+str(options.publish)

print "will submit:"
print crabSubmitCmd
print " "

if options.dryRun=="False":
    print "submitting for REAL"
    import time
    time.sleep(5)
    import os
    os.system(crabSubmitCmd)
    import shutil
    if os.path.isdir(crabProjDir+"/crab_"+workingDirTmp):
        shutil.move(crabProjDir+"/crab_"+workingDirTmp,crabProjDir+"/"+workingDir)

print "All done"
