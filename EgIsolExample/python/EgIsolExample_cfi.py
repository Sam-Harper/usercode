import FWCore.ParameterSet.Config as cms


egIsolExample = cms.EDAnalyzer("EgIsolExample",
                               electronTag=cms.InputTag("pixelMatchGsfElectrons"),
                               ecalIsolTag=cms.InputTag("eleIsoFromDepsEcalFromHits"),
                               hcalIsolTag=cms.InputTag("eleIsoFromDepsHcalFromTowers"),
                               trackIsolTag=cms.InputTag("eleIsoFromDepsTk"),
                               hcalDepth1IsolTag=cms.InputTag("eleIsoFromDepsHcalDepth1FromTowers"),
                               hcalDepth2IsolTag=cms.InputTag("eleIsoFromDepsHcalDepth1FromTowers"),
)

