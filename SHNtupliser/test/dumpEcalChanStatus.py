isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
process = cms.Process("HEEP")

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 


options.parseArguments()

print options.inputFiles
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles),  
                          )



# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(5000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

#Load geometry
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.autoCond import autoCond
from Configuration.AlCa.GlobalTag import GlobalTag

process.GlobalTag = GlobalTag(process.GlobalTag, '105X_mc2017_realistic_v5', '')


# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)

process.chanDumper= cms.EDAnalyzer("DumpEcalChanStatus",
                                   name=cms.string("ecalChanStatus"))

process.path = cms.Path(process.chanDumper)
