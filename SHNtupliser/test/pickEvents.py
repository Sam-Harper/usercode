#!/usr/bin/env python

#mostly stolen from edmPickEvents.py developed by
# Anzar Afaq         June 17, 2008
# Oleksiy Atramentov June 21, 2008
# Charles Plager     Sept  7, 2010
# Volker Adler       Apr  16, 2014

def splitListEvenly(orgList,nrJobs,jobNr):
    newList=[]
    for index in range(0,len(orgList)):
        if (index+jobNr)%nrJobs==0: newList.append(orgList[index])

    return newList

#this splits the list into continuous sections
#in the case it doesnt divide evenly, the first X jobs have an extra entry
def splitListSections(orgList,nrJobs,jobNr):
    jobNr=jobNr%nrJobs #rescaling it to 0 to nrJobs-1
    nrEntries=len(orgList)
    nrEntriesPerJob = int(nrEntries/nrJobs)
    leftOverJobs = nrEntries%nrJobs
    offset = min(jobNr,leftOverJobs)
    
    startEntry = jobNr*nrEntriesPerJob+offset
    endEntry = startEntry+nrEntriesPerJob-1
    if jobNr<leftOverJobs:
        endEntry+=1

    return orgList[startEntry:endEntry+1]


import argparse
import os

#from FWCore.ParameterSet.VarParsing import VarParsing


parser = argparse.ArgumentParser(description='gives the files and optional picks the events given')
parser.add_argument('pythonFile',help='python file')
parser.add_argument('dataset',help='name of dataset')
parser.add_argument('input',help='either input file name containing run/lumi/event or runnr:lumiSec:eventnr')
parser.add_argument('output',help='actually skim the events and output in this file (None if no output)')
parser.add_argument('nrJobs',default=1,help='number of jobs to split into')
parser.add_argument('jobNr',default=1,help='job number')
args = parser.parse_args()
print args

##import sys
##args={}
##args['dataset']=sys.argv[1]
##args['input']=sys.argv[2]
##args['output']=None

from SHarper.SHNtupliser.dasFileQuery import *

eventList = []

Event.dataset = args.dataset
import re
commentRE = re.compile (r'#.+$')

if os.path.isfile(args.input):
    print "file detected"
    eventsFile = open(args.input,'r')
    for line in eventsFile:
        line = commentRE.sub ('', line)
        try:
            event = Event (line)
        except:
            print "Skipping '%s'." % line.strip()
            continue
#        print event["event"]
        eventList.append(event)
    eventsFile.close()
else:
    inputCleaned = args.input.replace(":"," ")
    try:   
        event = Event (inputCleaned)
    except:
        print "Skipping '%s'." % inputCleaned
    eventList.append(event)

eventList = splitListSections(eventList,int(args.nrJobs),int(args.jobNr))


filesSet=set([])
fileCache={}
for event in eventList:
    for filename in getFileNamesCache (event,fileCache):
        filesSet.add(filename)

#print filesSet
files=list(filesSet)

#print files
#print fileCache

if args.output=="None":
    for filename in files:
        print filename
#    print files
    import FWCore.ParameterSet.Config as cms
    process = cms.Process("Skim")
    process.source = cms.Source("PoolSource",
                        
                                fileNames = cms.untracked.vstring(),
                                eventsToProcess = cms.untracked.VEventRange()
                                )
else:
    outputFilename = args.output
    if args.nrJobs!=1:
        outputFilename=outputFilename.rstrip(".root")+"_"+str(args.jobNr)+".root"

    # Import configurations
    import FWCore.ParameterSet.Config as cms

    # set up process
    process = cms.Process("Skim")

 
    process.source = cms.Source("PoolSource",
                        
                                fileNames = cms.untracked.vstring(),
                                eventsToProcess = cms.untracked.VEventRange()
                                )
    for filename in files:
        print filename
        process.source.fileNames.extend([str(filename),])

    #print "event list "
    for event in eventList:
        runEventNr=str(event["run"])+":"+str(event["event"])
        process.source.eventsToProcess.extend([runEventNr+"-"+runEventNr,])

        # set the number of events
    process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(-1)
        )

    
    process.load('Configuration/EventContent/EventContent_cff')
    process.output = cms.OutputModule("PoolOutputModule",
                                      splitLevel = cms.untracked.int32(0),
                                      #  outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
                                      # outputCommands = process.AODSIMEventContent.outputCommands,
                                      outputCommands = cms.untracked.vstring("keep *"),
                                      fileName = cms.untracked.string(outputFilename),
                                      
                                      dataset = cms.untracked.PSet(
                                          dataTier = cms.untracked.string('HLTDEBUG'),                                     
                                          )
                                      )


    process.output_step = cms.EndPath(process.output)   
    
