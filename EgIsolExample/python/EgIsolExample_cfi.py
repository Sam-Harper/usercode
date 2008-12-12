import FWCore.ParameterSet.Config as cms


egIsolExample = cms.EDAnalyzer("EgIsolExample",
                               electronTag=cms.InputTag("pixelMatchGsfElectrons"),
                               ecalIsolFromDepsTag=cms.InputTag("eleIsoFromDepsEcalFromHits"),
                               hcalIsolFromDepsTag=cms.InputTag("eleIsoFromDepsHcalFromTowers"),
                               trackIsolFromDepsTag=cms.InputTag("eleIsoFromDepsTk"),
                               hcalDepth1IsolFromDepsTag=cms.InputTag("eleIsoFromDepsHcalDepth1FromTowers"),
                               hcalDepth2IsolFromDepsTag=cms.InputTag("eleIsoFromDepsHcalDepth2FromTowers"),
                               ecalIsolTag=cms.InputTag("electronEcalRecHitIsolationLcone"),
                               hcalIsolTag=cms.InputTag("electronHcalTowerIsolationLcone"),
                               trackIsolTag=cms.InputTag("electronTrackIsolationLcone"),
                               hcalDepth1IsolTag=cms.InputTag("electronHcalDepth1TowerIsolationLcone"),
                               hcalDepth2IsolTag=cms.InputTag("electronHcalDepth2TowerIsolationLcone"),
)

