# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("TrigRate")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkSummary = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)

import sys
filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

if(sys.argv[2].find("/castor/")==0):
    filePrefex="rfio:"
process.source = cms.Source("PoolSource",
                          fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                          #  fileNames = cms.untracked.vstring(),
)

# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Photon30_CaloIdVL_v*")
process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT2")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("SHarper.TrigTools.trigRateNtupMaker_cfi")
process.trigRateNtupMaker.outputFilename= sys.argv[3]
process.trigRateNtupMaker.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT2")
#these are the filters which the P4s are going to be saved for
process.trigRateNtupMaker.filterNames = cms.vstring("hltEG30CaloIdVLHEFilter",
                                                "hltJet10Open",
                                                "hltMet05",
                                                "hltMHT05",
                                                "hltMHT05Pt20")


process.p = cms.Path(#process.skimHLTFilter*                  
                     process.trigRateNtupMaker)
