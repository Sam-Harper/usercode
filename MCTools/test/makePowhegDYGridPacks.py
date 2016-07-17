

def makeGridPack(args,massPair):
    if args.baseConfig.find("_ee_")==-1 or args.baseConfig.find("_13TeV")==-1:
        print "config file name ",args.baseConfig," miss named, cant insert the mass range nor get process \n",
        exit

    with open(args.baseConfig,"r") as f:
        configLines = f.readlines()
        
    processName=args.baseConfig.split("_ee_")[0]
    if processName=="Z_ew-BMNNPV":
        configFilename=args.baseConfig.replace("_13TeV","_13TeV_M_"+str(massPair[0]))
    else:
        configFilename=args.baseConfig.replace("_13TeV","_13TeV_M_"+str(massPair[0])+"_"+str(massPair[1]))
    if args.withNegWeights==1:
        configFilename=configFilename.replace(".input","_withNegWeights.input")
    configFilename=configFilename.replace("_base","")
    outputDir=configFilename.replace(".input","")

    with open(configFilename,"w") as configFile:
        for line in configLines:
            line=line.replace("TOSED:MASSLOW",str(massPair[0]))
            line=line.replace("TOSED:MASSHIGH",str(massPair[1]))
            line=line.replace("TOSED:WITHNEGWEIGHTS",str(args.withNegWeights))
            configFile.write(line)


    import subprocess
    jobArgs=['python','./run_pwg.py','-p','f','-i',configFilename,'-m',processName,'-f',outputDir,'-n',str(args.nrEvents)]
   # print configFilename+"\n",  #the \n and the , are important here
  #  output= subProcess.Popen(jobArgs,stdout=subprocess.PIPE).communicate()[0]
#    output= subProcess.Popen(['python','./run_pwg.py','-p','f','-i','Z_ew-BMNNPV_ee_M2000_NNPDF30_13TeV.input','-m','Z_ew-BMNNPV','-f','Z_ew-BMNNPV_Test5','-n','1000'],stdout=subprocess.PIPE).communicate()[0]

    

import threading
class JobThread (threading.Thread):
    def __init__(self, args,threadId,massPairs):
        threading.Thread.__init__(self)
        self.args=args
        self.threadId=threadId
        self.massPairs=massPairs
    def run(self):
        nrJobs = len(self.massPairs)
        for jobNr in range(0,nrJobs):
            if (jobNr+self.threadId)%self.args.nrThreads==0:
                makeGridPack(self.args,self.massPairs[jobNr])



import os
import argparse

parser = argparse.ArgumentParser(description='LHE analyser')

parser.add_argument('baseConfig',help='base config filename')#,nargs="+")
parser.add_argument('--nrEvents',default=1000,type=int,help='number of events to gen')
parser.add_argument('--withNegWeights',default=0,type=int,help='with negative weights')
parser.add_argument('--nrThreads',default=-1,type=int,help='number of threads to run')
args = parser.parse_args()


masses=[[50,120],[120,200],[200,400],[400,800],[800,1400],[1400,2300],[2300,3500],[3500,4500],[4500,6000],[6000,-1]]

   
threads=[]

args.nrThreads = args.nrThreads
if args.nrThreads ==-1: args.nrThreads=len(masses)
print "nr threads ",args.nrThreads
for threadId in range(0,args.nrThreads):
    threads.append(JobThread(args,threadId,masses))

for thread in threads:
    thread.start()   
