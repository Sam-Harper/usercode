import FWCore.ParameterSet.Config as cms

from EgammaAnalysis.ElectronTools.regressionModifier_cfi import regressionModifier

gedGsfElectrons = cms.EDProducer("ModifiedGsfElectronProducer",
    src = cms.InputTag("gedGsfElectrons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet() )
)
 
gedPhotons = cms.EDProducer("ModifiedRecoPhotonProducer",
    src = cms.InputTag("gedPhotons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet() )
)    

egamma_modifications = cms.VPSet( )
egamma_modifications.append( regressionModifier )

gedGsfElectrons.modifierConfig.modifications = egamma_modifications
gedPhotons.modifierConfig.modifications   = egamma_modifications

regressionApplication = cms.Sequence( gedGsfElectrons * gedPhotons )
