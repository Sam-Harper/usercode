import FWCore.ParameterSet.Config as cms

from RecoEgamma.EgammaTools.regressionModifier_cfi import regressionModifier



gedGsfElectronsReg = cms.EDProducer("ModifiedGsfElectronProducer",
    src = cms.InputTag("gedGsfElectrons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet() )
)
 
gedPhotonsReg = cms.EDProducer("ModifiedRecoPhotonProducer",
    src = cms.InputTag("gedPhotons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet() )
)    

egamma_modifications = cms.VPSet( )
egamma_modifications.append( regressionModifier )

gedGsfElectronsReg.modifierConfig.modifications = egamma_modifications
gedPhotonsReg.modifierConfig.modifications   = egamma_modifications

regressionApplication = cms.Sequence( gedGsfElectronsReg * gedPhotonsReg )
