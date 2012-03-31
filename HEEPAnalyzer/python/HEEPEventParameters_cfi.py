import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *

heepEventPara  = cms.PSet (
    electronTag = cms.untracked.InputTag("patElectrons"),
    tauTag = cms.untracked.InputTag("patTaus"),
    muonTag = cms.untracked.InputTag("patMuons"),
    jetTag = cms.untracked.InputTag("patJets"),
    photonTag = cms.untracked.InputTag("patPhotons"),
    metTag = cms.untracked.InputTag("patMETs"),
    barrelRecHitTag = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
    endcapRecHitTag = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
    reducedBarrelRecHitTag = cms.InputTag("reducedEcalRecHitsEB"),
    reducedEndcapRecHitTag = cms.InputTag("reducedEcalRecHitsEE"),
    hbheRecHitsTag = cms.InputTag("hbhereco"),
    gsfEleTag = cms.InputTag("gsfElectrons"),
    recoPhoTag = cms.InputTag("photons"),
    superClusterEBTag = cms.InputTag("correctedHybridSuperClusters"),
    superClusterEETag = cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"),
    ctfTrackTag = cms.InputTag("generalTracks"),
    genParticleTag = cms.InputTag("genParticles"),
    trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    trigResultsTag = cms.InputTag("TriggerResults","","HLT"),
    genEventInfoTag = cms.InputTag("generator"),
    pileUpMCInfoTag = cms.InputTag("addPileupInfo"),
    l1RecordTag = cms.InputTag("gtDigis"),
    l1EmNonIsoTag = cms.InputTag("hltL1extraParticles","NonIsolated"),
    l1EmIsoTag =   cms.InputTag("hltL1extraParticles","Isolated"), 
    verticesTag = cms.InputTag("offlinePrimaryVerticesWithBS"),
    caloTowersTag = cms.InputTag("towerMaker"),
    eleRhoCorrTag = cms.InputTag("kt6PFJets","rho"),
    hltProcName = cms.string("HLT"), #HLT process name, always HLT unless it was rerun...
    maxDRTrigMatch = cms.double(0.2), #max DR for a reco object trigger object match
    maxPtRelDiffTrigMatch = cms.double(1.), #max relative difference in pt between reco object and trig obj
    hltFiltersToCheck = cms.vstring(),#only these filters are checked
    barrelCuts = cms.PSet(heepBarrelCuts),
    endcapCuts = cms.PSet(heepEndcapCuts),
    applyRhoCorrToEleIsol = cms.bool(False), 
    eleIsolEffectiveAreas = cms.PSet (
        trackerBarrel = cms.double(0.),
        trackerEndcap = cms.double(0.),
        ecalBarrel = cms.double(0.101),
        ecalEndcap = cms.double(0.046),
        hcalBarrel = cms.double(0.021),
        hcalEndcap = cms.double(0.040)
        ),
    onlyAddEcalDriven = cms.bool(True), #only promote ecal driven (ie not tracker driven PF electrons) to heep electrons
    heepEleSource = cms.int32(0) #0 = GsfElectrons, 1 = pat::Electrons to make the heep electrons from 
    
   
)


