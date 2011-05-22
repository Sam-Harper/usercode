import FWCore.ParameterSet.Config as cms

trigRateNtupMaker = cms.EDAnalyzer("TriggerRateNtupMaker",
                                   outputFilename = cms.string("testOutput.root"),
                                   trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
                                   filterNames = cms.vstring()
                                   )
                                   
