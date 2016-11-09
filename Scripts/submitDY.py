#!/usr/bin/env python


import threading
import subprocess
class JobThread (threading.Thread):
    def __init__(self, cmds):
        threading.Thread.__init__(self)
        self.cmds=cmds.split()
        self.stdout=None
        self.stderr=None
    def run(self):
#        print self.cmds,
        import subprocess
        self.stdout,self.stderr = subprocess.Popen(self.cmds,stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()
        

import argparse
import os

parser = argparse.ArgumentParser(description='submits CMSSW jobs to RAL batch system')
parser.add_argument('--config',help='cmsRun config file to run',required=True)
#parser.add_argument('--output',help='output filebase name, defaults to outputDir+.root',default=None)
parser.add_argument('--outputDir',help='ouput dir (under scratch/mc/CMSSWVersion/<outputdir>',required=True)
parser.add_argument('--baseOutDir',help='base output directory',default="mc")
args = parser.parse_args()


inputFiles=["ZToEE_NNPDF30_13TeV-powheg_M_50_120_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1_RAWAODSIM.list",
            "ZToEE_NNPDF30_13TeV-powheg_M_120_200_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1_RAWAODSIM.list",
            "ZToEE_NNPDF30_13TeV-powheg_M_200_400_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1_RAWAODSIM.list",
            "ZToEE_NNPDF30_13TeV-powheg_M_400_800_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1_RAWAODSIM.list",
            "ZToEE_NNPDF30_13TeV-powheg_M_800_1400_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1_RAWAODSIM.list",
            "ZToEE_NNPDF30_13TeV-powheg_M_1400_2300_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1_RAWAODSIM.list",
            "ZToEE_NNPDF30_13TeV-powheg_M_2300_3500_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v5_RAWAODSIM.list",
            "ZToEE_NNPDF30_13TeV-powheg_M_3500_4500_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1_RAWAODSIM.list",
            "ZToEE_NNPDF30_13TeV-powheg_M_4500_6000_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1_RAWAODSIM.list",
            "ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf_RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1_RAWAODSIM.list"]


baseDir="/opt/ppd/scratch/harper"
baseOutputDir=baseDir+"/"+args.baseOutDir
cmsswVersion=os.environ['CMSSW_VERSION']
fullOutputDir=baseOutputDir+"/"+cmsswVersion.split("CMSSW_")[1]+"/"+args.outputDir
if os.path.exists(fullOutputDir):
    print "output directory ",fullOutputDir," exists, aborting "
    exit(1)
os.makedirs(fullOutputDir)

threads=[]
for filelist in inputFiles:
    outputFilename=fullOutputDir.rstrip("/")+"/"+filelist.split("_RunIISpring16")[0]+"_ntup_SHv29pre2.root"
    cmd="cmsRun %s %s %s" % (args.config,filelist,outputFilename)
    threads.append(JobThread(cmd))

for thread in threads:
    thread.start()   

while(len(threads)!=0):
    for thread in threads:
        if not thread.isAlive():
            print "job","finished"
#            print thread.cmds
            print thread.stdout
            print thread.stderr
            
            threads.remove(thread)


print "all jobs done"
    

