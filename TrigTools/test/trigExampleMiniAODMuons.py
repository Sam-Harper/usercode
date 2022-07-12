# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("TrigExample")

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

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
#right now we are only reading the L1 menu from this so any data tag will do
#of course you shoudl replace with the recommended one for your data when running "for real"
process.GlobalTag = GlobalTag(process.GlobalTag, "124X_dataRun3_Prompt_v1", '')

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring("/store/data/Run2018B/DoubleMuon/MINIAOD/UL2018_MiniAODv2-v1/260000/B976B625-3E16-8E40-9E0A-8180729E2821.root"),
)

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.trigExample = cms.EDAnalyzer("MiniAODTriggerMuonExample",           
                                     trigObjs = cms.InputTag("slimmedPatTrigger"),
                                     trigResults = cms.InputTag("TriggerResults","","HLT"),
                                     datasets = cms.vstring("SingleMuon","DoubleMuon"),
                                     muons = cms.InputTag("slimmedMuons"),
                                     stageL1Trigger = cms.uint32(2)
                                  )



process.p = cms.Path(process.trigExample)
