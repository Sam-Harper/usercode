
import argparse
import json
import os

parser = argparse.ArgumentParser(description='gives the files and optional picks the events given')
parser.add_argument('--dataset',help='name of dataset')
parser.add_argument('--runnr',help='runnr')
parser.add_argument('--lumis',help='lumis seperated by :')
parser.add_argument('--outputDir',default="./",help='output directory')
args = parser.parse_args()
print args

from SHarper.SHNtupliser.dasFileQuery import *

lumis=""
if os.path.isfile(args.lumis):
    with open(args.lumis) as data_file:    
        lumi_data = json.load(data_file)
    for lumi_range in lumi_data[args.runnr]:
        for lumi in range(lumi_range[0],lumi_range[1]+1):
            lumis+=str(lumi)+":"
    lumis=lumis.rstrip(":")
    print lumis
   
else:

    lumis=args.lumis
    if len(args.lumis.split("-"))==2:
        lumis=""
        for lumiNr in range(int(args.lumis.split("-")[0]),int(args.lumis.split("-")[1])+1):
            lumis+=str(lumiNr)+":"
            lumis=lumis.rstrip(":")

print lumis

eventList=[]
Event.dataset = args.dataset
import re
commentRE = re.compile (r'#.+$')
for lumi in lumis.split(":"):
    eventStr=str(args.runnr)+":"+lumi+":1"
    try:
        event = Event (eventStr)
    except:
        print "Skipping '%s'." % inputCleaned
    eventList.append(event)

filesSet=set([])
fileCache={}
for event in eventList:
    for filename in getFileNamesCache (event,fileCache):
        filesSet.add(filename)

for filename in filesSet:
    fileLumis = sorted(getLumis(filename)[0])
    
    
    fileData={'dataset' : args.dataset.replace("/","_").lstrip("_"),'runnr' : args.runnr,'minLS' : fileLumis[0],'maxLS' : fileLumis[-1]  }
    outputFilename = "{d[dataset]}_{d[runnr]}_LS{d[minLS]}to{d[maxLS]}.root".format(d=fileData)
    print fileLumis,filename,outputFilename
    
    cpArgs="xrdcp root://xrootd-cms.infn.it//"+filename+" "+args.outputDir.rstrip("/")+"/"+outputFilename
#    fullEOSFilename=str("/eos/cms/tier0/"+filename)
 #   eosCpArgs=['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','cp',fullEOSFilename,args.outputDir.rstrip("/")+"/"+outputFilename]
    print cpArgs
    import subprocess
    out,err = subprocess.Popen(cpArgs.split(),stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()
    print out
    print err
    #for line in err.splitline():
    #    print line

