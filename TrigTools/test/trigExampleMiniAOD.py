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
                            fileNames = cms.untracked.vstring("/store/data/Run2018A/Tau/MINIAOD/17Sep2018-v1/60000/BADFCDEF-F1FF-A743-A79F-BF4D64D65B9B.root")
)

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.trigExample = cms.EDAnalyzer("MiniAODTriggerExample",           
                                     trigObjs = cms.InputTag("slimmedPatTrigger"),
                                     trigResults = cms.InputTag("TriggerResults","","HLT"),
                                     filtersToPass = cms.vstring("hltDoublePFTau40TrackPt1MediumChargedIsolationAndTightOOSCPhotonsDz02Reg"),
                                     pathsToPass = cms.vstring("HLT_DoubleMediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_v",),
                                     eles = cms.InputTag("slimmedElectrons")
                                  )



process.p = cms.Path(process.trigExample)
