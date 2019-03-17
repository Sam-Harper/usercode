#!/usr/bin/env python

def splitInput(inputFilesRAW,nrJobs):
    import glob

    if ".list" in inputFilesRAW:
        inputFiles= [line.strip() for line in open(inputFilesRAW)]
    else:
        inputFiles=glob.glob(inputFilesRAW)
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
    return inputFilesEachJob


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
        output= subprocess.Popen(cmd.split(),stdout=subprocess.PIPE).communicate()[0].splitlines()

import time
import argparse
parser = argparse.ArgumentParser(description='runs multiple instances of given cmd')
parser.add_argument('cmd',help='cmd to run')
parser.add_argument('--inputFiles',help='input file or file pattern to run over, replaces %{inputFiles} string')
parser.add_argument('--nrJobs',help='number of jobs to split into (can not be larger than #files to run over), replaces %{jobNr} string)',default=1,type=int)
parser.add_argument('--nrThreads','-t',type=int,default=-1,help='number of python threads to use')
parser.add_argument('--seperator','-s',help='seperator of files on the cmd line, usually space or ,',default=' ',type=str)
args = parser.parse_args()

inputFilesEachJob=[""]*args.nrJobs
if args.inputFiles is not None:
    inputFilesEachJob = splitInput(args.inputFiles,args.nrJobs)

if args.nrThreads <=0:
    args.nrThreads = args.nrJobs

threads=[]
for jobNr in range(1,args.nrJobs+1):
    while len(threads)>=args.nrThreads:
      #  print "waiting for job {}".format(jobNr)
        time.sleep(10)
        for thread in threads:
            if not thread.isAlive():
                threads.remove(thread)
    print "starting job {}, nr running {}".format(jobNr,len(threads))
    inputFiles=""
    for inFile in inputFilesEachJob[jobNr%args.nrJobs]:
        inputFiles+=inFile+args.seperator
    inputFiles=inputFiles[:len(args.seperator)*-1] #chop the last seperator off
   # print inputFiles
    threads.append(JobThread(args,jobNr,inputFiles))
    threads[-1].start()

while len(threads)!=0:
    time.sleep(10)
    for thread in threads:
        if not thread.isAlive():
            threads.remove(thread)
    
