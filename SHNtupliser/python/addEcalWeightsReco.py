# Import configurations
import FWCore.ParameterSet.Config as cms

def addEcalWeightsReco(process,path,indexToInsert):


    process.load('RecoLocalCalo.EcalRecProducers.ecalWeightUncalibRecHit_cfi')
    process.ecalWeightUncalibRecHit.EBdigiCollection = cms.InputTag("selectDigi","selectedEcalEBDigiCollection")
    process.ecalWeightUncalibRecHit.EEdigiCollection = cms.InputTag("selectDigi","selectedEcalEEDigiCollection")
    process.load('RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi')
    process.ecalWeightsRecHit = process.ecalRecHit.clone()
    process.ecalWeightsRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalWeightUncalibRecHit","EcalUncalibRecHitsEE")
    process.ecalWeightsRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalWeightUncalibRecHit","EcalUncalibRecHitsEB")
    process.ecalWeightsRecHit.recoverEBFE = cms.bool(False)
    process.ecalWeightsRecHit.recoverEEFE = cms.bool(False)
    process.ecalWeightsRecHit.killDeadChannels = cms.bool(False)

    
    process.ecalWeightsSeq = cms.Sequence(process.ecalWeightUncalibRecHit * process.ecalWeightsRecHit )

    path.insert(indexToInsert,process.ecalWeightsSeq)
