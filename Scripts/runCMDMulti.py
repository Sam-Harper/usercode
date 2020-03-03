#!/usr/bin/env python
import sys

def splitInput(inputFilesRAW,nrJobsRAW):
    import glob
    
    
    if ".list" in inputFilesRAW:
        inputFiles= [line.strip() for line in open(inputFilesRAW)]
    else:
        inputFiles=glob.glob(inputFilesRAW)
    
    if nrJobsRAW<=0:
        nrJobs = len(inputFiles)
    else:
        nrJobs=nrJobsRAW

#    print inputFiles
    inputFilesEachJob=nrJobs*[None];

    nrFiles=len(inputFiles)
    nrFilesPerJob=nrFiles/nrJobs
    nrLeftOverFiles=nrFiles%nrJobs
    
    for jobNr in range(0,nrJobs):
        minFileNr=jobNr*nrFilesPerJob
        maxFileNr=(jobNr+1)*nrFilesPerJob
        inputFilesEachJob[jobNr]=inputFiles[minFileNr:maxFileNr]

    for leftFileNr in range(0,nrLeftOverFiles):
        inputFilesEachJob[leftFileNr].append(inputFiles[(jobNr+1)*nrFilesPerJob+leftFileNr])
    return inputFilesEachJob,nrJobs


import threading
class JobThread (threading.Thread):
    def __init__(self, args,jobNr,inputFiles):
        threading.Thread.__init__(self)
        self.args=args
        self.jobNr=jobNr
        self.inputFiles=inputFiles
    def run(self):
        cmd = args.cmd
        cmd = cmd.replace("%{inputFiles}",self.inputFiles)
        cmd = cmd.replace("%{jobNr}",str(self.jobNr))
        import subprocess
#        print cmd
        output= subprocess.Popen(cmd.split(),stdout=subprocess.PIPE).communicate()[0].splitlines()

import time
import argparse
parser = argparse.ArgumentParser(description='runs multiple instances of given cmd')
parser.add_argument('cmd',help='cmd to run')
parser.add_argument('--inputFiles',help='input file or file pattern to run over, replaces %{inputFiles} string',required=True)
parser.add_argument('--nrJobs',help='number of jobs to split into (can not be larger than #files to run over), replaces %{jobNr} string)',default=1,type=int)
parser.add_argument('--nrThreads','-t',type=int,default=-1,help='number of python threads to use')
parser.add_argument('--seperator','-s',help='seperator of files on the cmd line, usually space or ,',default=' ',type=str)
args = parser.parse_args()

if args.cmd.find("%{jobNr}")==-1 and args.nrJobs!=1:
    print "error, no %{jobNr} found in cmd string and number of jobs is not 1"
    sys.exit()
if args.cmd.find("%{inputFiles}")==-1 and args.inputFiles:
    print "error, no %{inputFiles} found in cmd string and inputFiles is specificed"
    sys.exit()
    

inputFilesEachJob=[""]*args.nrJobs
nrJobs = args.nrJobs
if args.inputFiles is not None:
    inputFilesEachJob,nrJobs = splitInput(args.inputFiles,nrJobs)

if args.nrThreads <=0:
    args.nrThreads = nrJobs
print inputFilesEachJob
threads=[]
print nrJobs
for jobNr in range(1,nrJobs+1):
    while len(threads)>=args.nrThreads:
#        print "waiting for job {}".format(jobNr)
        time.sleep(10)
        for thread in threads:
            if not thread.isAlive():
                threads.remove(thread)
    print "starting job {}, nr running {}".format(jobNr,len(threads))
    inputFiles=""
    for inFile in inputFilesEachJob[jobNr%nrJobs]:
        inputFiles+=inFile+args.seperator
    inputFiles=inputFiles[:len(args.seperator)*-1] #chop the last seperator off
    print inputFiles

    threads.append(JobThread(args,jobNr,inputFiles))
    threads[-1].start()

while len(threads)!=0:
    time.sleep(10)
    for thread in threads:
        if not thread.isAlive():
            threads.remove(thread)
    
