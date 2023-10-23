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
parser.add_option('--grl',help='good run lumi json',default="")
options,args = parser.parse_args()
if not options.input or not options.pattern or not options.shNtupVersion or not options.cmsswVersion or not options.config:
    parser.error("input, pattern, shNtupVersion, config and cmsswVersion are manditory")
print(options.config)

crabProjDir='crab_projects'

datasetDefFile =open(options.input)
for line in datasetDefFile:
    if line.find(options.pattern)==-1 or line.find(options.antipattern)!=-1:
        continue

    splitLine = line.split("&")
    datasetPath = splitLine[0].rstrip(" ")
    datasetFormat = datasetPath.split("/")[-1]
     


    dataset=datasetPath.split("/")[1]
    datasetId = datasetPath.split("/")[2]



    runStart = int(splitLine[1])
    runEnd = int(splitLine[2])
    runRange = str(runStart)+"-"+str(runEnd)

    totalUnits = -1
    unitsPerJob = int(splitLine[3])
#    nrJobs=int(splitLine[6])
    datasetCode=0 #always zero for data

    outputFile=dataset+"_ntuples_"+datasetId+"_"+options.shNtupVersion+".root"
    outputPath="/store/user/sharper/"+options.cmsswVersion+"/"+options.shNtupVersion+"/"+datasetFormat
    publishDataname=datasetId+"_"+datasetFormat+"_"+options.shNtupVersion+"_"+runRange
    # /store/user/<dir>[/<subdirs>]/<primary-dataset>/<publication-name>/<time-stamp>/<counter>/<file-name>
    import datetime
    currTime=datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
#    datasetId = "HEmiss2"
    workingDir="Data_"+dataset+"_"+options.cmsswVersion+"_"+options.shNtupVersion+"_"+runRange+"_"+datasetFormat+"_"+datasetId+"_"+currTime


    print(workingDir,"working dir length",len(workingDir))
    #print datasetPath,nrJobs,datasetCode,dataset,datasetFormat
    #print publishDataname
    #print outputFile,outputPath
    
    if datasetFormat.find("MINIAOD")!=-1:
        useMiniAODStr="useMiniAOD=True"
    else:
        useMiniAODStr="useMiniAOD=False"

    with open(options.config,"r") as configFile:
        configLines = configFile.readlines()
   
    tempConfig="cmssw_temp.py"
    with open(tempConfig,"w") as tempConfigFile:
        for line in configLines:
            line=line.replace("OUTPUTFILE",outputFile)
            line=line.replace("SAMPLEWEIGHT","1")
            line=line.replace("isCrabJob=False","isCrabJob=True");
            line=line.replace("#MINIAODTYPE",useMiniAODStr);
            line=line.replace("DATASETCODE",str(datasetCode))
            line=line.replace("TOSED:DATASETNAME",str(dataset))
            line=line.replace("TOSED:DATASETVERSION",str(datasetPath.split("/")[2]))
            tempConfigFile.write(line)

  
    grlStr=""
    if options.grl!="":
        print("adding grl",options.grl)
        grlStr = " Data.lumiMask='"+options.grl+"'"
        
    crabSubmitCmd = "crab submit -c crab_base.py General.requestName="+workingDir+ \
                    " General.transferOutputs="+str(options.copyData)+ \
                    " Data.inputDataset="+datasetPath+ \
                    " Data.inputDBS="+options.dbsUrl+ \
                    " Data.unitsPerJob="+str(unitsPerJob)+ \
                    " Data.totalUnits="+str(totalUnits)+ \
                    " Data.outLFNDirBase="+outputPath+ \
                    " Data.ignoreLocality="+str(options.ignoreLocality)+ \
                    " Data.outputDatasetTag="+publishDataname+ \
                    " Data.runRange="+runRange+ \
                    " JobType.psetName="+tempConfig+ \
                    " General.workArea="+crabProjDir+ \
                    " General.transferLogs="+str(options.transferLogFiles)+ \
                    grlStr
    print("will submit:")
    print(crabSubmitCmd)
    print(" ")

    #so we can redo our cmd later easily
    scriptCmd = sys.argv[0]+" "
    for entry in sys.argv[1:]: scriptCmd+=entry+" "

    if options.dryRun=="False":
        print("submitting for REAL")
        time.sleep(5)
        os.system(crabSubmitCmd)
        full_crab_dir = crabProjDir+"/crab_"+workingDir
        if os.path.isdir(full_crab_dir):
            with open(full_crab_dir+"/submitCmd","w") as f:
                f.write(scriptCmd+"\n")
            with open(full_crab_dir+"/crabCmd","w") as f:
                f.write(crabSubmitCmd+"\n")
                
            shutil.copy(tempConfig,full_crab_dir+"/cmssw.py")
        

print("All done")
