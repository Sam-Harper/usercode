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
                            fileNames = cms.untracked.vstring("file:/opt/ppd/month/harper/dataFiles/DoubleEG_Run2017F_MINIAOD_94X_305064_3E907453-F3E1-E711-BFB4-0CC47AB0B704.root")
)

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.trigExample = cms.EDAnalyzer("Ele32DoubleL1ToSingleL1Example",           
                                     trigObjs = cms.InputTag("slimmedPatTrigger"),
                                     trigResults = cms.InputTag("TriggerResults","","HLT"),
                                    
                                     eles = cms.InputTag("slimmedElectrons")
                                  )



process.p = cms.Path(process.trigExample)
