#!/usr/bin/env python

import os
import optparse

parser = optparse.OptionParser(description='submits jobs to crab')

parser.add_option('--input',help='file listing the dataset')
parser.add_option('--pattern',help='pattern to match in input file')
parser.add_option('--antipattern',help='pattern not to match input file',default="ewerwefewf")
parser.add_option('--dryRun',help="if true, is a dry run, do not actually submit crab jobs",default=True)
parser.add_option('--config1',help='cmsRun config file to run',default="shNtupliser_mc_basic.py")
parser.add_option('--config2',help='cmsRun config file to run',default="shNtupliser_mc_basic.py")
parser.add_option('--script',help='script to run',default="script.sh")
parser.add_option('--shNtupVersion',help='shNtup version')
parser.add_option('--cmsswVersion',help='cmssw version')
parser.add_option('--copyData',help="whether to stage out the data or not",default=True)
parser.add_option('--dbsUrl',help="dbs url global,phys03 etc",default="global")
parser.add_option('--transferLogFiles',help="whether to transfer log files or not",default=False)
options,args = parser.parse_args()
if not options.input or not options.pattern or not options.shNtupVersion or not options.cmsswVersion or not options.config1 or not options.config2 or not options.script:
    parser.error("input, pattern, shNtupVersion, config1, config2, script and cmsswVersion are manditory")
print options.config1


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


    #print datasetPath,nrJobs,datasetCode,dataset,datasetFormat
    #print publishDataname
    #print outputFile,outputPath

    with open(options.config1,"r") as configFile:
        configLines1 = configFile.readlines()
   
    with open(options.config2,"r") as configFile:
        configLines2 = configFile.readlines()

    tempConfig1="cmssw_temp.py"
    with open(tempConfig1,"w") as tempConfigFile:
        for line in configLines1:
            line=line.replace("OUTPUTFILE","tmpOutput.root")
            line=line.replace("SAMPLEWEIGHT","1")
            line=line.replace("isCrabJob=False","isCrabJob=True");
            line=line.replace("DATASETCODE",str(datasetCode))
            tempConfigFile.write(line)

    tempConfig2="cmssw_temp2.py"
    with open(tempConfig2,"w") as tempConfigFile:
        for line in configLines2:
            line=line.replace("OUTPUTFILE",outputFile)
            line=line.replace("SAMPLEWEIGHT","1")
            line=line.replace("isCrabJob=False","isCrabJob=True");
            line=line.replace("DATASETCODE",str(datasetCode))
            tempConfigFile.write(line)
    
    with open("crab_base.py","r") as crabFile:
        crabLines = crabFile.readlines()      

    tempCrab="crab_base_temp.py"
    with open(tempCrab,"w") as tempCrabFile:
        for line in crabLines:
            extraJobInfo = "config.JobType.inputFiles=[\"%s\"] \nconfig.JobType.outputFiles=[\"%s\"] \nconfig.JobType.disableAutomaticOutputCollection=True" % (tempConfig2,outputFile)
            line=line.replace("#TOSED:EXTRAJOBTYPEINFO",extraJobInfo);
            tempCrabFile.write(line)
    
    crabSubmitCmd = "crab submit -c crab_base_temp.py General.requestName="+workingDir+ \
                    " General.transferOutputs="+str(options.copyData)+ \
                    " Data.inputDataset="+datasetPath+ \
                    " Data.inputDBS="+options.dbsUrl+ \
                    " Data.unitsPerJob="+str(unitsPerJob)+ \
                    " Data.totalUnits="+str(totalUnits)+ \
                    " Data.outLFNDirBase="+outputPath+ \
                    " Data.publishDataName="+publishDataname+ \
                    " JobType.psetName="+tempConfig1+ \
                    " JobType.scriptExe="+options.script+ \
                    " General.transferLogs="+str(options.transferLogFiles) 
                    
    print "will submit:"
    print crabSubmitCmd
    print " "

    if options.dryRun=="False":
        print "submitting for REAL"
        import time
        time.sleep(5)
        import os
        os.system(crabSubmitCmd)

print "All done"
