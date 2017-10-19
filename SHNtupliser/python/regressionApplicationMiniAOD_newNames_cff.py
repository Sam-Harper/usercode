import FWCore.ParameterSet.Config as cms

from RecoEgamma.EgammaTools.regressionModifier_cfi import regressionModifier

slimmedElectronsReg = cms.EDProducer("ModifiedElectronProducer",
    src = cms.InputTag("slimmedElectrons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet() )
)
 
slimmedPhotonsReg = cms.EDProducer("ModifiedPhotonProducer",
    src = cms.InputTag("slimmedPhotons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet() )
)    

egamma_modifications = cms.VPSet( )
egamma_modifications.append( regressionModifier )

slimmedElectronsReg.modifierConfig.modifications = egamma_modifications
slimmedPhotonsReg.modifierConfig.modifications   = egamma_modifications

regressionApplication = cms.Sequence( slimmedElectronsReg * slimmedPhotonsReg )
