import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *

heepEventPara  = cms.PSet (
    #intentionally using allLayer1 objects rather than selectedLayer1
    #because quite frankly I dont trust the selection
    #note that layer1 already has cleaning and selection cuts applied to it
    electronTag = cms.untracked.InputTag("allLayer1Electrons"),
    tauTag = cms.untracked.InputTag("allLayer1Taus"),
    muonTag = cms.untracked.InputTag("allLayer1Muons"),
    jetTag = cms.untracked.InputTag("allLayer1Jets"),
    photonTag = cms.untracked.InputTag("allLayer1Photons"),
    metTag = cms.untracked.InputTag("allLayer1METs"),
    barrelRecHitTag = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
    endcapRecHitTag = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
    reducedBarrelRecHitTag = cms.InputTag("reducedEcalRecHitsEB"),
    reducedEndcapRecHitTag = cms.InputTag("reducedEcalRecHitsEE"),
    hbheRecHitsTag = cms.InputTag("hbhereco"),
    superClusterEBTag = cms.InputTag("correctedHybridSuperClusters"),
    superClusterEETag = cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"),
    ctfTrackTag = cms.InputTag("generalTracks"),
    genParticleTag = cms.InputTag("genParticles"),
    trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    hltProcName = cms.string("HLT"), #HLT process name, always HLT unless it was rerun...
    maxDRTrigMatch = cms.double(0.2), #max DR for a reco object trigger object match
    maxPtRelDiffTrigMatch = cms.double(1.), #max relative difference in pt between reco object and trig obj
    hltFiltersToCheck = cms.vstring('hltL1NonIsoHLTNonIsoSingleElectronLWEt15TrackIsolFilter',
                                    'hltL1NonIsoHLTNonIsoSingleElectronEt15TrackIsolFilter',
                                    'hltL1NonIsoSingleEMHighEtTrackIsolFilter',
                                    'hltL1NonIsoSinglePhotonEMVeryHighEtEtFilter'),#only these filters are checked
    barrelCuts = cms.PSet(heepBarrelCuts),
    endcapCuts = cms.PSet(heepEndcapCuts)
   
)


