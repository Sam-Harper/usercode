
from RecoParticleFlow.PFClusterProducer.particleFlowRecHitECAL_cfi import particleFlowRecHitECAL
from RecoParticleFlow.PFClusterProducer.particleFlowClusterECALUncorrected_cfi import particleFlowClusterECALUncorrected
from RecoEcal.EgammaClusterProducers.particleFlowSuperClusterECAL_cfi import particleFlowSuperClusterECAL

emptyEEESAssoc = cms.Producer("EmptyESAssocProducer")
particleFlowRecHitECAL.producers[0].qualityTests = cms.VPSet( 
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
particleFlowRecHitECAL.producers[1].qualityTests = cms.VPSet( 
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

particleFlowSuperClusterECAL.ESAssociation = cms.InputTag("emptyEEESAssoc")
particleFlowSuperClusterECAL.ESAssociation.PFClusters = cms.InputTag("particleFlowClusterECALUncorrected"),
rePFSuperClusterTask = cms.Task(particleFlowRecHitECAL*particleFlowClusterECALUncorrected*emptyEEESAssoc*particleFlowSuperClusterECA)
rePFSuperClusterSeq = cms.Sequence(rePFSuperClusterTask)
