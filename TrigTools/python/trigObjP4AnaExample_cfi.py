import FWCore.ParameterSet.Config as cms

trigObjP4AnaExample = cms.EDAnalyzer("TrigObjP4AnaExample",
                                  
                                   trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
                                   filterName = cms.string("hltPhoton135HEFilter"),
                                   pathName=cms.string("HLT_DoubleEle33_CaloIdT_v1")
                                   )
                                   
