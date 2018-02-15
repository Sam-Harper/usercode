#!/usr/bin/env python

import os
import optparse
import sys
import time
import shutil

parser = optparse.OptionParser(description='submits jobs to crab')

parser.add_option('--input',help='file listing the dataset')
parser.add_option('--pattern',help='pattern to match in input file')
parser.add_option('--antipattern',help='pattern not to match input file',default="ewerwefewf")
parser.add_option('--dryRun',help="if true, is a dry run, do not actually submit crab jobs",default=True)
parser.add_option('--config',help='cmsRun config file to run',default="shNtupliser_mc_basic.py")
parser.add_option('--shNtupVersion',help='shNtup version')
parser.add_option('--cmsswVersion',help='cmssw version')
parser.add_option('--copyData',help="whether to stage out the data or not",default=True)
parser.add_option('--dbsUrl',help="dbs url global,phys03 etc",default="global")
parser.add_option('--transferLogFiles',help="whether to transfer log files or not",default=False)
parser.add_option('--ignoreLocality',help="whether to ignore locality",default=False)
parser.add_option('--crabDryRun',help='crab dry run',default=False)
options,args = parser.parse_args()
if not options.input or not options.pattern or not options.shNtupVersion or not options.cmsswVersion or not options.config:
    parser.error("input, pattern, shNtupVersion, config and cmsswVersion are manditory")
print options.config


crabProjDir='crab_projects'

datasetDefFile =open(options.input)
for line in datasetDefFile:
    if line.find(options.pattern)==-1 or line.find(options.antipattern)!=-1:
        continue

    splitLine = line.split("&")
    datasetPath = splitLine[0].rstrip(" ")
    datasetFormat = datasetPath.split("/")[-1]
    

 #   print datasetPath,nrJobs,datasetCode
    dataset=datasetPath.split("/")[1]
    datasetId = datasetPath.split("/")[2]
    datasetId = datasetId[datasetId.find("-")+1:]

    nrEvents = int(splitLine[1])
    if nrEvents!=-1:
        totalUnits = int(nrEvents/100)
    else:
        totalUnits = -1
    
    unitsPerJob = int(splitLine[6])
#    nrJobs=int(splitLine[6])
    datasetCode=int(splitLine[5])

    outputFile=dataset+"_ntuples_"+datasetId+"_"+options.shNtupVersion+".root"
    outputPath="/store/user/sharper/"+options.cmsswVersion+"/"+options.shNtupVersion+"/"+datasetFormat
    publishDataname=datasetId+"_"+datasetFormat+"_"+options.shNtupVersion
    # /store/user/<dir>[/<subdirs>]/<primary-dataset>/<publication-name>/<time-stamp>/<counter>/<file-name>
    import datetime
    currTime=datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
    workingDir="MC_"+dataset+"_"+options.cmsswVersion+"_"+options.shNtupVersion+"_"+datasetFormat+"_"+datasetId+"_"+currTime
    workingDirTmp="MC_"+dataset+currTime

    with open(options.config,"r") as configFile:
        configLines = configFile.readlines()
   
    tempConfig="cmssw_temp.py"
    with open(tempConfig,"w") as tempConfigFile:
        for line in configLines:
            line=line.replace("OUTPUTFILE",outputFile)
            line=line.replace("SAMPLEWEIGHT","1")
            line=line.replace("isCrabJob=False","isCrabJob=True");
            line=line.replace("DATASETCODE",str(datasetCode))
            tempConfigFile.write(line)

  
    
    crabSubmitCmd = "crab submit -c crab_base.py --wait General.requestName="+workingDirTmp+ \
                    " General.transferOutputs="+str(options.copyData)+ \
                    " Data.inputDataset="+datasetPath+ \
                    " Data.inputDBS="+options.dbsUrl+ \
                    " Data.unitsPerJob="+str(unitsPerJob)+ \
                    " Data.totalUnits="+str(totalUnits)+ \
                    " Data.outLFNDirBase="+outputPath+ \
                    " Data.ignoreLocality="+str(options.ignoreLocality)+ \
                    " Data.outputDatasetTag="+publishDataname+ \
                    " JobType.psetName="+tempConfig+ \
                    " General.workArea="+crabProjDir+ \
                    " General.transferLogs="+str(options.transferLogFiles)
    if options.crabDryRun=="True":
        crabSubmitCmd=crabSubmitCmd.replace("--wait","--dryrun")
    print "will submit:"
    print crabSubmitCmd
    print " "
    
    #so we can redo our cmd later easily
    scriptCmd = sys.argv[0]+" "
    for entry in sys.argv[1:]: scriptCmd+=entry+" "

    if options.dryRun=="False":
        print "submitting for REAL"
        time.sleep(5)
        os.system(crabSubmitCmd)
        tmp_crab_dir = crabProjDir+"/crab_"+workingDirTmp
        full_crab_dir = crabProjDir+"/"+workingDir
        if os.path.isdir(tmp_crab_dir):
            shutil.move(tmp_crab_dir,full_crab_dir)
            with open(full_crab_dir+"/submitCmd","w") as f:
                f.write(scriptCmd+"\n")
            with open(full_crab_dir+"/crabCmd","w") as f:
                f.write(crabSubmitCmd+"\n")
                
            shutil.copy(tempConfig,full_crab_dir+"/cmssw.py")

        

print "All done"
