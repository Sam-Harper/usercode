import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *

def swapHEEPToMiniAOD(heepPara):
    heepPara.electronTag = cms.InputTag("slimmedElectrons")
    heepPara.tauTag = cms.InputTag("slimmedTaus")
    heepPara.muonTag = cms.InputTag("slimmedMuons")
    heepPara.jetTag = cms.InputTag("slimmedJets")
    heepPara.photonTag = cms.InputTag("slimmedPhotons")
    heepPara.metTag = cms.InputTag("slimmedMETs")
    heepPara.gsfEleTag = cms.InputTag("slimmedElectrons")
    heepPara.oldGsfEleTag = cms.InputTag("slimmedElectrons",processName=cms.InputTag.skipCurrentProcess())
    heepPara.recoPhoTag = cms.InputTag("slimmedPhotons")
    heepPara.oldPhoTag = cms.InputTag("slimmedPhotons",processName=cms.InputTag.skipCurrentProcess())
    heepPara.reducedBarrelRecHitTag = cms.InputTag("reducedEgamma","reducedEBRecHits")
    heepPara.reducedEndcapRecHitTag = cms.InputTag("reducedEgamma","reducedEERecHits")
    heepPara.superClusterEBTag = cms.InputTag("reducedEgamma","reducedSuperClusters")
    heepPara.superClusterEETag = cms.InputTag("reducedEgamma","reducedSuperClusters")
    heepPara.preShowerClusterXTag = cms.InputTag("reducedEgamma","reducedESClusters")
    heepPara.preShowerClusterYTag = cms.InputTag("reducedEgamma","reducedESClusters")
    heepPara.verticesTag = cms.InputTag("offlineSlimmedPrimaryVertices")
    heepPara.pileUpMCInfoTag = cms.InputTag("slimmedAddPileupInfo")
    heepPara.genParticleTag = cms.InputTag("prunedGenParticles")
heepEventPara  = cms.PSet (
    electronTag = cms.InputTag("patElectrons"),
    tauTag = cms.InputTag("patTaus"),
    muonTag = cms.InputTag("patMuons"),
    jetTag = cms.InputTag("patJets"),
    photonTag = cms.InputTag("patPhotons"),
    metTag = cms.InputTag("patMETs"),
    barrelRecHitTag = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
    endcapRecHitTag = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
    barrelWeightsRecHitTag = cms.InputTag("ecalGlobalRecHitSelectedDigis","EcalRecHitsEB"),
    endcapWeightsRecHitTag = cms.InputTag("ecalGlobalRecHitSelectedDigis","EcalRecHitsEE"),
    reducedBarrelRecHitTag = cms.InputTag("reducedEcalRecHitsEB"),
    reducedEndcapRecHitTag = cms.InputTag("reducedEcalRecHitsEE"),
    hbheRecHitsTag = cms.InputTag("hbhereco"),
    gsfEleTag = cms.InputTag("gedGsfElectrons"),
    oldGsfEleTag = cms.InputTag("gedGsfElectrons",processName=cms.InputTag.skipCurrentProcess()),
    recoPhoTag = cms.InputTag("gedPhotons"),
    oldPhoTag = cms.InputTag("gedPhotons",processName=cms.InputTag.skipCurrentProcess()),
    pfCandidateTag = cms.InputTag("particleFlow"),
    packedPFCandTag = cms.InputTag("packedPFCandidates"),
    lostTrackTag = cms.InputTag("lostTracks"),
    lostEleTrackTag = cms.InputTag("lostTracks:eleTracks"),
    superClusterEBTag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALBarrel"),
    superClusterEETag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALEndcapWithPreshower"),
    preShowerClusterXTag = cms.InputTag("multi5x5SuperClustersWithPreshower","preshowerXClusters"),
    preShowerClusterYTag = cms.InputTag("multi5x5SuperClustersWithPreshower","preshowerYClusters"),
    ctfTrackTag = cms.InputTag("generalTracks"),
    genParticleTag = cms.InputTag("genParticles"),
    packedGenParticleTag = cms.InputTag("packedGenParticles"),
    trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    patTrigObjsTag=cms.InputTag("slimmedPatTrigger"),
    trigResultsTag = cms.InputTag("TriggerResults","","HLT"),
    genEventInfoTag = cms.InputTag("generator"),
    pileUpMCInfoTag = cms.InputTag("addPileupInfo"),
    l1RecordTag = cms.InputTag("gtDigis"),
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
    eleIsolPtTrksValueMapTag = cms.InputTag("heepIDVarValueMaps","eleTrkPtIso"),
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
    heepIDVID = cms.InputTag("egmGsfElectronIDs","heepElectronID-HEEPV70"),
    heepIDVIDBits = cms.InputTag("egmGsfElectronIDs","heepElectronID-HEEPV70Bitmap"),
    vidBits = cms.VInputTag("egmGsfElectronIDs:heepElectronID-HEEPV70Bitmap",
                            "egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-vetoBitmap",
                            "egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-looseBitmap",
                            "egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-mediumBitmap",
                            "egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-tightBitmap",
                            "egmGsfElectronIDs:mvaEleID-Fall17-noIso-V1-wp90Bitmap",
                            "egmGsfElectronIDs:mvaEleID-Fall17-noIso-V1-wp80Bitmap",
                            "egmGsfElectronIDs:mvaEleID-Fall17-noIso-V1-wpLooseBitmap",
                            "egmGsfElectronIDs:mvaEleID-Fall17-iso-V1-wp90Bitmap",
                            "egmGsfElectronIDs:mvaEleID-Fall17-iso-V1-wp80Bitmap",
                            "egmGsfElectronIDs:mvaEleID-Fall17-iso-V1-wpLooseBitmap"),
    lheEventTag=cms.InputTag("externalLHEProducer"),
    genEvtInfoTag=cms.InputTag("generator"),
    trkIsoNoJetCoreTag=cms.InputTag("heepIDVarValueMaps","eleTrkPtIso"),
    nrSatCrysIn5x5Tag=cms.InputTag("heepIDVarValueMaps","eleNrSaturateIn5x5"),
    beamSpotTag=cms.InputTag("offlineBeamSpot"),
    gsFixDupECALClusters = cms.InputTag("particleFlowEGammaGSFixed","dupECALClusters"),
    gsFixHitsNotReplaced = cms.InputTag("ecalMultiAndGSGlobalRecHitEB","hitsNotReplaced"),
    gsFixOrgReducedEGEBHits = cms.InputTag("reducedEgammaBeforeGSFix","reducedEBRecHits"),
    gsFixMETOrg = cms.InputTag("slimmedMETs"),
    gsFixMETEGClean = cms.InputTag("slimmedMETsEGClean"),
    extraJetTags = cms.VInputTag(),
    extraMETTags = cms.VInputTag(),

    
)


