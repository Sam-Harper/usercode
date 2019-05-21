import FWCore.ParameterSet.Config as cms

from RecoParticleFlow.PFClusterProducer.particleFlowRecHitECAL_cfi import particleFlowRecHitECAL as _particleFlowRecHitECAL
from RecoParticleFlow.PFClusterProducer.particleFlowClusterECALUncorrected_cfi import particleFlowClusterECALUncorrected as _particleFlowClusterECALUncorrected
from RecoEcal.EgammaClusterProducers.particleFlowSuperClusterECAL_cfi import particleFlowSuperClusterECAL as _particleFlowSuperClusterECAL  

particleFlowRecHitECALNoThres = _particleFlowRecHitECAL.clone()
particleFlowClusterECALUncorrectedNoThres = _particleFlowClusterECALUncorrected.clone(recHitsSource=cms.InputTag("particleFlowRecHitECALNoThres"))
particleFlowSuperClusterECALNoThres = _particleFlowSuperClusterECAL.clone()

particleFlowClusterECALNoThres = cms.EDProducer("ECALPFClusterWithDummyCorrProducer",
                                                src = cms.InputTag("particleFlowClusterECALUncorrectedNoThres")
)

emptyEEESAssoc = cms.EDProducer("EmptyESAssocProducer")
particleFlowRecHitECALNoThres.producers[0].src = "reducedEcalRecHitsEB"
particleFlowRecHitECALNoThres.producers[0].qualityTests = cms.VPSet( 
    cms.PSet(
        name = cms.string("PFRecHitQTestThreshold"),
        threshold = cms.double(0.08)
    ),
    cms.PSet(
        name = cms.string("PFRecHitQTestECAL"),
        cleaningThreshold = cms.double(2.0),
        timingCleaning = cms.bool(True),
        topologicalCleaning = cms.bool(True),
        skipTTRecoveredHits = cms.bool(True)
    )
)
particleFlowRecHitECALNoThres.producers[1].src = "reducedEcalRecHitsEE"
particleFlowRecHitECALNoThres.producers[1].qualityTests = cms.VPSet( 
    cms.PSet(
        name = cms.string("PFRecHitQTestThreshold"),
        threshold = cms.double(0.3)
    ),
    cms.PSet(
        name = cms.string("PFRecHitQTestECAL"),
        cleaningThreshold = cms.double(2.0),
        timingCleaning = cms.bool(True),
        topologicalCleaning = cms.bool(True),
        skipTTRecoveredHits = cms.bool(True)
    )
)

particleFlowSuperClusterECALNoThres.ESAssociation = "particleFlowClusterECALNoThres"
particleFlowSuperClusterECALNoThres.PFClusters = "particleFlowClusterECALNoThres"
particleFlowSuperClusterECALNoThres.regressionConfig.ecalRecHitsEB = "reducedEcalRecHitsEB"
particleFlowSuperClusterECALNoThres.regressionConfig.ecalRecHitsEE = "reducedEcalRecHitsEE"
rePFSuperClusterTask = cms.Task(particleFlowRecHitECALNoThres, particleFlowClusterECALUncorrectedNoThres, particleFlowClusterECALNoThres, particleFlowSuperClusterECALNoThres)
rePFSuperClusterThresSeq = cms.Sequence(rePFSuperClusterTask)
