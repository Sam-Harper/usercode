import FWCore.ParameterSet.Config as cms


gsfElectronsHEEPCorr = cms.EDProducer("HEEPGsfEleEnergyCorrector",
                                      inputEleTag = cms.InputTag("gsfElectrons"),
                                      useSuperClusterEnergy = cms.bool(True)
                                      )

