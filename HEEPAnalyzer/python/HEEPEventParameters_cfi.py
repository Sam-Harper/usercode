import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *

def swapHEEPToMiniAOD(heepPara):
    heepPara.electronTag = cms.untracked.InputTag("slimmedElectrons")
    heepPara.tauTag = cms.untracked.InputTag("slimmedTaus")
    heepPara.muonTag = cms.untracked.InputTag("slimmedMuons")
    heepPara.jetTag = cms.untracked.InputTag("slimmedJets")
    heepPara.photonTag = cms.untracked.InputTag("slimmedPhotons")
    heepPara.metTag = cms.untracked.InputTag("slimmedMETs")
    heepPara.gsfEleTag = cms.InputTag("slimmedElectrons")
    heepPara.recoPhoTag = cms.InputTag("slimmedPhotons")
    heepPara.reducedBarrelRecHitTag = cms.InputTag("reducedEgamma","reducedEBRecHits")
    heepPara.reducedEndcapRecHitTag = cms.InputTag("reducedEgamma","reducedEBRecHits")
    heepPara.superClusterEBTag = cms.InputTag("reducedEgamma","reducedSuperClusters")
    heepPara.superClusterEETag = cms.InputTag("reducedEgamma","reducedSuperClusters")
    heepPara.preShowerClusterXTag = cms.InputTag("reducedEgamma","reducedESClusters")
    heepPara.preShowerClusterYTag = cms.InputTag("reducedEgamma","reducedESClusters")
    heepPara.heepIDVID = cms.InputTag("egmGsfElectronIDs","heepElectronID-HEEPV51-miniAOD")
    heepPara.heepEleSource = cms.int32(1)
    heepPara.verticesTag = cms.InputTag("offlineSlimmedPrimaryVertices")
    heepPara.pileUpMCInfoTag = cms.InputTag("slimmedAddPileupInfo")
    
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
    gsfEleTag = cms.InputTag("gedGsfElectrons"),
    recoPhoTag = cms.InputTag("gedPhotons"),
    pfCandidateTag = cms.InputTag("particleFlow"),
    packedPFCandTag = cms.InputTag("packedPFCandidates"),
    lostTrackTag = cms.InputTag("lostTracks"),
    superClusterEBTag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALBarrel"),
    superClusterEETag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALEndcapWithPreshower"),
    preShowerClusterXTag = cms.InputTag("multi5x5SuperClustersWithPreshower","preshowerXClusters"),
    preShowerClusterYTag = cms.InputTag("multi5x5SuperClustersWithPreshower","preshowerYClusters"),
    ctfTrackTag = cms.InputTag("generalTracks"),
    genParticleTag = cms.InputTag("genParticles"),
    trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    trigResultsTag = cms.InputTag("TriggerResults","","HLT"),
    genEventInfoTag = cms.InputTag("generator"),
    pileUpMCInfoTag = cms.InputTag("addPileupInfo"),
    l1RecordTag = cms.InputTag("gtDigis"),
    l1EmNonIsoTag = cms.InputTag("hltL1extraParticles","NonIsolated"),
    l1EmIsoTag =   cms.InputTag("hltL1extraParticles","Isolated"), 
    egHLTCandsTag = cms.InputTag("hltEgammaCandidates"),
    egHLTCandsUnseededTag = cms.InputTag("hltEgammaCandidatesUnseeded"),
    verticesTag = cms.InputTag("offlinePrimaryVertices"),
    caloTowersTag = cms.InputTag("towerMaker"),
    eleRhoCorrTag = cms.InputTag("fixedGridRhoFastjetAll"),
    eleRhoCorr2012Tag = cms.InputTag("kt6PFJets","rho"),
    ecalLaserFilter = cms.InputTag("ecalLaserCorrFilter"),
    pfChargedIsoValEleMapTag = cms.InputTag("elPFIsoValueCharged03PFIdPFIso"),
    pfNeutralIsoValEleMapTag = cms.InputTag("elPFIsoValueNeutral03PFIdPFIso"),
    pfPhotonIsoValEleMapTag = cms.InputTag("elPFIsoValueGamma03PFIdPFIso"),
    hltProcName = cms.string("HLT"), #HLT process name, always HLT unless it was rerun...
    maxDRTrigMatch = cms.double(0.2), #max DR for a reco object trigger object match
    maxPtRelDiffTrigMatch = cms.double(1.), #max relative difference in pt between reco object and trig obj
    hltFiltersToCheck = cms.vstring(),#only these filters are checked
    barrelCuts = cms.PSet(heepBarrelCuts),
    endcapCuts = cms.PSet(heepEndcapCuts),
    applyRhoCorrToEleIsol = cms.bool(True),
    onlyAddEcalDriven = cms.bool(True), #only promote ecal driven (ie not tracker driven PF electrons) to heep electrons
    heepEleSource = cms.int32(0), #0 = GsfElectrons, 1 = pat::Electrons to make the heep electrons from
    pfClustersECALTag = cms.InputTag("particleFlowClusterECAL"),
    pfClustersHCALTag = cms.InputTag("particleFlowClusterHCAL"), 
    gsfEleToPFCandMapTag = cms.InputTag("particleBasedIsolation","gedGsfElectrons"),
    heepIDVID = cms.InputTag("egmGsfElectronIDs","heepElectronID-HEEPV51"),
    lheEventTag=cms.InputTag("externalLHEProducer"),
    genEvtInfoTag=cms.InputTag("generator"),
    trkIsoNoJetCoreTag=cms.InputTag(""),
    nrSatCrysIn5x5Tag=cms.InputTag(""),
    beamSpotTag=cms.InputTag("offlineBeamSpot")
)


