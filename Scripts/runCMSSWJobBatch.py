#!/usr/env python

import shutil
import os

def buildTar_(tgzNameWithPath):
    swArea=os.environ['CMSSW_BASE']
    import tarfile
    tar = tarfile.open(tgzNameWithPath, "w:gz")
    lib = swArea+'/' +"lib"
    if os.path.exists(lib):
        print "adding ",lib
        tar.add(lib)
        print "added ",lib

    for package in os.listdir(swArea+"/src"):
        if os.path.isdir(swArea+"/src/"+package):
            for subPackage in os.listdir(swArea+"/src/"+package):
                if os.path.isdir(swArea+"/src/"+package+"/"+subPackage):
                     for subDir in os.listdir(swArea+"/src/"+package+"/"+subPackage):
                         if subDir=='data':
                             tar.add("src/"+package+"/"+subPackage+"/data");
                             print "adding dataDir: " + swArea+"/src/"+package+"/"+subPackage+"/data"
 

 
def copyReleaseFiles(newRelease):
    swArea=os.environ['CMSSW_BASE']
    lib = swArea+'/' +"lib"
    
    if os.path.exists(lib):
        #print "adding ",lib
        shutil.rmtree(newRelease+"/lib")
        shutil.copytree(lib,newRelease+"/lib")
        #print "added"
    if os.path.exists(swArea+"/python"):
        shutil.rmtree(newRelease+"/python");
        shutil.copytree(swArea+"/python",newRelease+"/python")
    for package in os.listdir(swArea+"/src"):
        if os.path.isdir(swArea+"/src/"+package):
            for subPackage in os.listdir(swArea+"/src/"+package):
                if os.path.isdir(swArea+"/src/"+package+"/"+subPackage):
                     for subDir in os.listdir(swArea+"/src/"+package+"/"+subPackage):
                         if subDir=='data':
                             os.makedirs(newRelease+"/src/"+package+"/"+subPackage)
                             shutil.copytree(swArea+"/src/"+package+"/"+subPackage+"/data",newRelease+"/src/"+package+"/"+subPackage+"/data")
                             print "adding dataDir: " + swArea+"/src/"+package+"/"+subPackage+"/data"
 
        



def splitInput(inputFilesRAW,nrJobs):
    import glob
    
    inputFiles=glob.glob(inputFilesRAW)
   # print inputFiles
    inputFilesEachJob=nrJobs*[None];

    nrFiles=len(inputFiles)
    nrFilesPerJob=nrFiles/nrJobs
    nrLeftOverFiles=nrFiles%nrJobs
    
    for jobNr in range(0,nrJobs):
        minFileNr=jobNr*nrFilesPerJob
        maxFileNr=(jobNr+1)*nrFilesPerJob
    #    print jobNr,minFileNr,maxFileNr
        inputFilesEachJob[jobNr]=inputFiles[minFileNr:maxFileNr]

    for leftFileNr in range(0,nrLeftOverFiles):
        inputFilesEachJob[leftFileNr].append(inputFiles[(jobNr+1)*nrFilesPerJob+leftFileNr])
                                 
   # print inputFilesEachJob
    return inputFilesEachJob

    
##def splitInput(inputFiles,nrJobs):
##    nrFiles=len(inputFiles)
##    nrFilesPerJob=nrFiles/nrJobs
##    nrLeftOverFiles=nrFiles%nrJobs
##    inputFilesEachJob=[[]]
##    for jobNr in range(1,nrJobs):
        
##        inputFiles[jobNr]
    

import sys
import os
import argparse

parser = argparse.ArgumentParser(description='submits CMSSW jobs to RAL batch system')
parser.add_argument('--config',help='cmsRun config file to run',required=True)
parser.add_argument('--input',help='input file or file pattern to run over',required=True)
parser.add_argument('--output',help='output filebase name',required=True)
parser.add_argument('--outputDir',help='ouput dir (under scratch/mc/CMSSWVersion/<outputdir>',required=True)
parser.add_argument('--nrJobs',help='number of jobs to split into (can not be larger than #files to run over)',default=1,type=int)
args = parser.parse_args()
print args.config

inputFilesForEachJob=splitInput(args.input,args.nrJobs)

baseOutputDir="/opt/ppd/scratch/harper/mc"
batchJobBaseDir="/opt/ppd/scratch/harper/cmsswBatchJobFiles/"
cmsswVersion=os.environ['CMSSW_VERSION']
swArea=os.environ['CMSSW_BASE']

batchSubmitBaseFile=swArea+"/src/SHarper/Scripts/qsub_base.sh"
batchSubmitFile="qsub_autoGen.sh"
#buildTar_("default.tgz")

print "config file ",args.config," base batch file ",batchSubmitBaseFile
    
fullOutputDir=baseOutputDir+"/"+cmsswVersion.split("CMSSW_")[1]+"/"+args.outputDir
if os.path.exists(fullOutputDir):
    print "output directory ",fullOutputDir," exists, aborting "
    exit(1)
os.makedirs(fullOutputDir)

import datetime
currTime=datetime.datetime.now().strftime('%Y%m%d_%H%M%S')

batchJobDir=cmsswVersion+"."+currTime
print batchJobDir

batchJobDirAndPath=batchJobBaseDir+batchJobDir


os.system("cd "+batchJobBaseDir+"; scramv1 proj -n "+batchJobDir+" CMSSW "+cmsswVersion)
#os.system("cp default.tgz "+batchJobDirAndPath+"/")
#os.system("tar -zxvf "+batchJobDirAndPath+"/default.tgz")
copyReleaseFiles(batchJobDirAndPath)

for jobNr in range(0,args.nrJobs):
    print jobNr
    batchSubmitFile="qsub_batch_"+str(jobNr)+".sh"
    outputFilename = args.output.split(".root")[0]+"_"+str(jobNr+1)+".root"
    shutil.copyfile(batchSubmitBaseFile,batchSubmitFile)
    shutil.copyfile(args.config,batchJobDirAndPath+"/src/cmssw.py")
    batchFile=open(batchSubmitFile,'a')
    batchFile.write("#job nr "+str(jobNr)+"\n")
    batchFile.write("cd "+batchJobDirAndPath+"/src\n");
    batchFile.write("echo $PWD")
    batchFile.write("cmsenv\n eval `scramv1 runtime -sh` \n");
    cmd="cmsRun cmssw.py"
    for filename in inputFilesForEachJob[jobNr]:
        cmd+=" "+filename
    cmd+=" $TMPDIR/"+outputFilename+"\n"
    cmd+="mv $TMPDIR/"+outputFilename+" "+fullOutputDir
    batchFile.write(cmd)
    batchFile.close()
    os.system("qsub "+batchSubmitFile+" -j oe -o /opt/ppd/scratch/harper/qsubLogs -q prod")
    os.remove(batchSubmitFile)        
