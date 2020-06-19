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
                            fileNames = cms.untracked.vstring("/store/data/Run2018D/EGamma/MINIAOD/22Jan2019-v2/60000/721D7ACD-5DDC-5941-8A30-D6A31914842A.root"),
)

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.trigExample = cms.EDAnalyzer("MiniAODTriggerExample",           
                                     trigObjs = cms.InputTag("slimmedPatTrigger"),
                                     trigResults = cms.InputTag("TriggerResults","","HLT"),
                                     filtersToPass = cms.vstring("hltDiEle33CaloIdLPixelMatchUnseededFilter","hltDiEle33CaloIdLMWPMS2UnseededFilter"),
                                     pathsToPass = cms.vstring("HLT_DoubleEle33_CaloIdL_MW_v",),
                                     eles = cms.InputTag("slimmedElectrons")
                                  )



process.p = cms.Path(process.trigExample)
