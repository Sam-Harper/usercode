import FWCore.ParameterSet.Config as cms

shNtupPara = cms.PSet(
    outputFilename = cms.string("testOutput.root"),
    datasetCode = cms.int32(0),
    sampleWeight = cms.double(1.),                       
    addMet = cms.bool(True),
    addJets = cms.bool(True),
    addMuons = cms.bool(False),
    addSuperClus = cms.bool(True),
    addEles = cms.bool(True),
    applyMuonId = cms.bool(True),
    addTrigs = cms.bool(True),
    addCaloTowers =cms.bool(False),
    addCaloHits =cms.bool(True),
    addIsolTrks =cms.bool(True),
    addPFCands = cms.bool(False),
    addPFClusters = cms.bool(False),
    addPUInfo = cms.bool(True),
    addPreShowerClusters =cms.bool(False),
    addGenInfo = cms.bool(True),
    addTrigSum = cms.bool(True),
    addHLTDebug = cms.bool(False),
    addMCParts = cms.bool(True),
    addPDFWeights = cms.bool(False),
    addGainSwitchInfo = cms.bool(False),
    addJetMETExtra= cms.bool(False),
    addEleUserData= cms.bool(False),
    fillTrkIsolFromUserData = cms.bool(False),
    trkIsolUserDataName = cms.string("heepTrkPtIso"),
    filterIsolTrks=cms.bool(True),
    filterEcalHits=cms.bool(True),
    filterHcalHits=cms.bool(True),
    filterCaloTowers=cms.bool(True),
    fillFromGsfEle = cms.bool(True),
    minEtToPromoteSC = cms.double(-1.),
    minEtToSaveEle = cms.double(-1),
    outputGeom = cms.bool(True),                  
  
   
    )
