
import FWCore.ParameterSet.Config as cms

egammaFilter = cms.EDFilter("EGammaFilter",
                            nrElesRequired=cms.int32(-1),
                            nrPhosRequired=cms.int32(-1),
                            nrSCsRequired=cms.int32(-1),
                            eleEtCut=cms.double(20),
                            phoEtCut=cms.double(20),
                            scEtCut=cms.double(-1),
                            eleTag=cms.InputTag("gedGsfElectrons"),
                            phoTag=cms.InputTag("gedPhotons"),
                            genEvtInfoTag=cms.InputTag("generator"),
                            superClusEBTag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALBarrel"),
                            superClusEETag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALEndcapWithPreshower"),
                            caloTowerTag = cms.InputTag("towerMaker"),
                            requireEcalDriven=cms.bool(False)
                            )
