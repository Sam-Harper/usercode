import FWCore.ParameterSet.Config as cms


gsfElectronsHEEPCorr = cms.EDProducer("HEEPGsfEleEnergyCorr",
                                      inputEleTag = cms.InputTag("gsfElectrons"),
                                      
                                      )

