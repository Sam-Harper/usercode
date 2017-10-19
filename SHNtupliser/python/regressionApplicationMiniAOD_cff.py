import FWCore.ParameterSet.Config as cms

from RecoEgamma.EgammaTools.regressionModifier_cfi import regressionModifier

slimmedElectrons = cms.EDProducer("ModifiedElectronProducer",
    src = cms.InputTag("slimmedElectrons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet() )
)
 
slimmedPhotons = cms.EDProducer("ModifiedPhotonProducer",
    src = cms.InputTag("slimmedPhotons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet() )
)    

egamma_modifications = cms.VPSet( )
egamma_modifications.append( regressionModifier )

slimmedElectrons.modifierConfig.modifications = egamma_modifications
slimmedPhotons.modifierConfig.modifications   = egamma_modifications

regressionApplication = cms.Sequence( slimmedElectrons * slimmedPhotons )
