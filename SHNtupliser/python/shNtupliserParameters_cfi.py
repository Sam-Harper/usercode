import FWCore.ParameterSet.Config as cms

shNtupPara = cms.PSet(
    outputFilename = cms.string("testOutput.root"),
    datasetCode = cms.int32(0),
    sampleWeight = cms.double(1.),                       
    addMet = cms.bool(True),
    addJets = cms.bool(True),
    addMuons = cms.bool(True),
    addSuperClus = cms.bool(True),
    addEles = cms.bool(True),
    applyMuonId = cms.bool(False),
    addTrigs = cms.bool(True),
    addCaloTowers =cms.bool(True),
    addCaloHits =cms.bool(True),
    addIsolTrks =cms.bool(True),
    addPFCands = cms.bool(False),
    addPFClusters = cms.bool(False),
    addPUInfo = cms.bool(False),
    addPreShowerClusters =cms.bool(False),
    addGenInfo = cms.bool(True),
    addTrigSum = cms.bool(True),
    addHLTDebug = cms.bool(False),
    filterIsolTrks=cms.bool(True),
    fillFromGsfEle = cms.bool(True),
    minEtToPromoteSC = cms.double(-1.),
    outputGeom = cms.bool(True),                  
  
   
    )
