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

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring("/store/data/Run2016B/DoubleEG/MINIAOD/23Sep2016-v3/60000/74DD13BA-E797-E611-BB4D-008CFA197DB8.root"),
)

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.trigExample = cms.EDAnalyzer("MiniAODTriggerExample",           
                                     trigObjs = cms.InputTag("selectedPatTrigger"),
                                     trigResults = cms.InputTag("TriggerResults","","HLT"),
                                     filtersToPass = cms.vstring("hltDiEle33CaloIdLPixelMatchUnseededFilter","hltDiEle33CaloIdLMWPMS2UnseededFilter"),
                                     pathsToPass = cms.vstring("HLT_DoubleEle33_CaloIdL_MW_v1",),
                                     eles = cms.InputTag("slimmedElectrons")
                                  )



process.p = cms.Path(process.trigExample)
