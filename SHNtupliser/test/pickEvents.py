#!/usr/bin/env python

#mostly stolen from edmPickEvents.py developed by
# Anzar Afaq         June 17, 2008
# Oleksiy Atramentov June 21, 2008
# Charles Plager     Sept  7, 2010
# Volker Adler       Apr  16, 2014

import argparse
import os

parser = argparse.ArgumentParser(description='gives the files and optional picks the events given')
parser.add_argument('pythonFile',help='python file')
parser.add_argument('dataset',help='name of dataset')
parser.add_argument('input',help='either input file name containing run/lumi/event or runnr:lumiSec:eventnr')
parser.add_argument('output',help='actually skim the events and output in this file (None if no output)')
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

files=[]
for event in eventList:
    files.extend( getFileNames (event) )

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
                                      fileName = cms.untracked.string(args.output),
                                      
                                      dataset = cms.untracked.PSet(
                                          dataTier = cms.untracked.string('HLTDEBUG'),                                     
                                          )
                                      )


    process.output_step = cms.EndPath(process.output)   
    
