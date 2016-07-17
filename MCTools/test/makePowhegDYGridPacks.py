def getPDFName(pdfSetNr):
    if pdfSetNr==260000: return "NNPDF30"
    elif pdfSetNr==10800: return "CT10"
    elif pdfSetNr==13100: return "CT14"
    elif pdfSetNr==90500: return "PDF4LHCmc"
    else: return "PDFSET"+str(pdfSetNr)

def makeGridPack(args,massPair):
    if args.baseConfig.find("_ee_")==-1 or args.baseConfig.find("_13TeV")==-1:
        print "config file name ",args.baseConfig," miss named, cant insert the mass range nor get process \n",
        exit

    with open(args.baseConfig,"r") as f:
        configLines = f.readlines()
        
    baseConfigName = args.baseConfig.split("/")[-1]
    processName=baseConfigName.split("_ee_")[0]
    if processName=="Z_ew-BMNNPV":
        configFilename=baseConfigName.replace("_13TeV","_13TeV_M_"+str(massPair[0]))
    else:
        if massPair[1]>=0: maxMassStr=str(massPair[1])
        else : maxMassStr="Inf"
        configFilename=baseConfigName.replace("_13TeV","_13TeV_M_"+str(massPair[0])+"_"+maxMassStr)
    if args.withNegWeights==1:
        configFilename=configFilename.replace(".input","_withNegWeights.input")
    configFilename=configFilename.replace("_base","")
    configFilename=configFilename.replace("_NNPDF30_","_"+getPDFName(args.pdfSetNr)+"_")
    outputDir=configFilename.replace(".input","")

    with open(configFilename,"w") as configFile:
        for line in configLines:
            line=line.replace("TOSED:MASSLOW",str(massPair[0]))
            line=line.replace("TOSED:MASSHIGH",str(massPair[1]))
            line=line.replace("TOSED:WITHNEGWEIGHTS",str(args.withNegWeights))
            line=line.replace("TOSED:PDFSETNR",str(args.pdfSetNr))
            configFile.write(line)


    import subprocess
    jobArgs=['python','./run_pwg.py','-p','f','-i',configFilename,'-m',processName,'-f',outputDir,'-n',str(args.nrEvents)]
    print configFilename+"\n",  #the \n and the , are important here
    output= subprocess.Popen(jobArgs,stdout=subprocess.PIPE).communicate()[0]

    

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
parser.add_argument('--pdfSetNr',default=260000,type=int,help='pdf set number (260000 NNPDF3.0, 10800 CT10, 13100 CT14, 90500 PDF4LHC mc')
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
