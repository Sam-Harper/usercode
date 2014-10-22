import os
import FWCore.ParameterSet.Config as cms
import sys
process = cms.Process( "HLTDUMP" )

#load path from confdb (for data --data, for mc --mc)
#I do the grep -v cms.EndPaths just to remove all the output modules

isMC=False
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkSummary = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(10000),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(10000),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.categories.append('TriggerSummaryProducerAOD')
process.MessageLogger.categories.append('L1GtTrigReport')
process.MessageLogger.categories.append('HLTrigReport')

process.hltTrigReport =  cms.EDAnalyzer("HLTrigReport",
                                HLTriggerResults = cms.InputTag("TriggerResults","",sys.argv[len(sys.argv)-1])
                                )
 
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)
# set process name
#process.setName_('HLT3')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = 'POSTLS161_V12::All'
#process.GlobalTag.globaltag = autoCond['com10']

#setup input /output modules, feel free to replace with your input/output settings

filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

if(sys.argv[2].find("/castor/")==0):
    filePrefex="rfio:"

process.source = cms.Source("PoolSource",
                        #  fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                            fileNames = cms.untracked.vstring(),
                            #eventsToProcess = cms.untracked.VEventRange("180250:97388063-180250:97388063")
                            )
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])

process.p = cms.Path(process.hltTrigReport) 
