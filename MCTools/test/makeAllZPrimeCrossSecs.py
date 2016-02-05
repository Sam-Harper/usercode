#!/usr/bin/env bash

def genZPrime(args,masses):
    for mass in masses:
        import subprocess
        massWin=int(0.05*13000)
        minMass=mass-massWin
        maxMass=mass+massWin
        outputFilename=args.model+"_13TeV_M-"+str(mass)+"_"+str(minMass)+"to"+str(maxMass)+".root"
        cmsRunArgs=["cmsRun",args.configFile,"zPrimeModel="+args.model,"maxEvents="+str(args.nrEvents),
                    "mass="+str(mass),"minMass="+str(minMass),"maxMass="+str(maxMass),
                    "outFile="+outputFilename]
        runStr=""
        
        #print cmsRunArgs
        output= subprocess.Popen(cmsRunArgs,stdout=subprocess.PIPE).communicate()[0].splitlines()
    

import threading
class JobThread (threading.Thread):
    def __init__(self, args,masses):
        threading.Thread.__init__(self)
        self.args=args
        self.masses=masses
    def run(self):
        genZPrime(self.args,self.masses)

def splitList(inputList,jobNr,nrJobs):
    outputList=[]
    for index,value in enumerate(inputList):
        if (jobNr+index)%nrJobs==0:
            outputList.append(value)
    return outputList

import os
import argparse
parser = argparse.ArgumentParser(description='generates Z model cross-sections using pythia')

parser.add_argument('configFile',help='starting directory')
parser.add_argument('--startMass',default=200,type=int,help='start mass')
parser.add_argument('--endMass',default=5000,type=int,help='end mass')
parser.add_argument('--massStep',default=25,type=int,help='mass step')
parser.add_argument('--model',required=True,help='Z\' model')
parser.add_argument('--nrEvents',default=1000,help='nrEvents to gen for each point')
parser.add_argument('--nrThreads',default=1,type=int,help='number of threads to run')
args = parser.parse_args()

masses=[]
for mass in range(args.startMass,args.endMass+args.massStep,args.massStep):
    masses.append(mass)

threads=[]
for threadId in range(0,args.nrThreads):
    massesForThread=list(splitList(masses,threadId,args.nrThreads))
    threads.append(JobThread(args,massesForThread))
    
for thread in threads:
    thread.start()   




