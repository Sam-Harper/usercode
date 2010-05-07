import FWCore.ParameterSet.Config as cms


import RecoEgamma.EgammaElectronProducers.ecalDrivenElectronSeeds_cfi
ecalDrivenElectronSeedsNoHE = RecoEgamma.EgammaElectronProducers.ecalDrivenElectronSeeds_cfi.ecalDrivenElectronSeeds.clone()
ecalDrivenElectronSeedsNoHE.SeedConfiguration.maxHOverE = cms.double(10000.)


import TrackingTools.GsfTracking.CkfElectronCandidateMaker_cff
electronCkfTrackCandidatesNoHE = TrackingTools.GsfTracking.CkfElectronCandidateMaker_cff.electronCkfTrackCandidates.clone()
#electronCkfTrackCandidatesNoHE.src = cms.InputTag('electronMergedSeedsNoHE')
electronCkfTrackCandidatesNoHE.src = cms.InputTag('ecalDrivenElectronSeedsNoHE')

import TrackingTools.GsfTracking.GsfElectronGsfFit_cff 
electronGsfTracksNoHE = TrackingTools.GsfTracking.GsfElectronGsfFit_cff.electronGsfTracks.clone()
electronGsfTracksNoHE.src = 'electronCkfTrackCandidatesNoHE'




gsfElectronCoresNoHE = cms.EDProducer("GsfElectronCoreProducer",
                                      gsfTracks =cms.InputTag("electronGsfTracksNoHE"),
                                      ctfTracks =cms.InputTag("generalTracks"),
                                      pfSuperClusters = cms.InputTag("pfElectronTranslator:pf"),
                                      pfSuperClusterTrackMap = cms.InputTag("pfElectronTranslator:pf")
                                      )
                                      
import RecoEgamma.EgammaElectronProducers.gsfElectrons_cfi
gsfElectronsNoHE = RecoEgamma.EgammaElectronProducers.gsfElectrons_cfi.gsfElectrons.clone()
gsfElectronsNoHE.gsfElectronCores = "gsfElectronCoresNoHE"
gsfElectronsNoHE.addPflowElectrons = cms.bool(False)
gsfElectronsNoHE.maxHOverEDepth1Barrel = cms.double(10000.)
gsfElectronsNoHE.maxHOverEDepth1Endcaps = cms.double(10000.)
gsfElectronsNoHE.maxHOverEDepth2 = cms.double(10000.)
gsfElectronsNoHE.applyPreselection = cms.bool(False)
#dont use this module as cant be bothered to figure how to rerun PF for now
electronMergedSeedsNoHE = cms.EDFilter("ElectronSeedMerger",
                                       EcalBasedSeeds =cms.InputTag("ecalDrivenElectronSeedsNoHE"),
                                       TkBasedSeeds = cms.InputTag("trackerDrivenElectronSeeds:SeedsForGsf")
                                       )
