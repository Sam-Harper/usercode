import FWCore.ParameterSet.Config as cms

heepAnalyzer = cms.EDAnalyzer("HEEPAnalyzer",
    electronTag = cms.untracked.InputTag("selectedLayer1Electrons"),
    tauTag = cms.untracked.InputTag("selectedLayer1Taus"),
    muonTag = cms.untracked.InputTag("selectedLayer1Muons"),
    jetTag = cms.untracked.InputTag("selectedLayer1Jets"),
    photonTag = cms.untracked.InputTag("selectedLayer1Photons"),
    metTag = cms.untracked.InputTag("selectedLayer1METs"),
    endcapRecHitCollection = cms.InputTag("reducedEcalRecHitsEE"),
    barrelRecHitCollection = cms.InputTag("reducedEcalRecHitsEB")  
                           
)


