import FWCore.ParameterSet.Config as cms

simTrigFilter = cms.EDFilter("SimTrigFilter",
                        
                               trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
                               trigCuts = cms.vstring()
                               )
                                   
