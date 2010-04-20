#shamelessly stolen from PatAnalyzerSkeleton_cfg.py

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkSummary = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('GR10_P_V4::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

#to allow me to re-run the tracking to redo electron seeds without H/E
process.load("RecoLocalTracker.SiPixelRecHits.SiPixelRecHits_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitConverter_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitMatcher_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.StripCPEfromTrackAngle_cfi")
process.load("RecoLocalTracker.SiStripZeroSuppression.SiStripZeroSuppression_cfi")
process.load("RecoLocalTracker.SiStripClusterizer.SiStripClusterizer_cfi")
process.load("RecoLocalTracker.SiPixelClusterizer.SiPixelClusterizer_cfi")
process.load("RecoLocalTracker.SiPixelRecHits.PixelCPEESProducers_cff")
process.load("RecoTracker.TransientTrackingRecHit.TTRHBuilders_cff")
process.load("RecoTracker.Configuration.RecoTracker_cff")
process.load("RecoTracker.IterativeTracking.FirstStep_cff")
process.load("SHarper.SHNtupliser.eleModulesNoHE_cfi")

import sys
process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = 1
process.shNtupliser.sampleWeight = 1
process.shNtupliser.outputFilename= sys.argv[3]
process.shNtupliser.gsfEleTag = "gsfElectrons"
process.shNtupliser.addMet = False
process.shNtupliser.addJets = False
process.shNtupliser.fillFromGsfEle = True

process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))
process.source.fileNames = cms.untracked.vstring("file:"+sys.argv[2],)

process.source.fileNames = cms.untracked.vstring("file:/media/disk/data10/skims/MinimumBias_RAW_HLTL1EG5Skim_132601_1_HLTReRunV30_V2_RECO.root",
"file:/media/disk/data10/skims/MinimumBias_RAW_HLTL1EG5Skim_132601_2_HLTReRunV30_V2_RECO.root",
"file:/media/disk/data10/skims/MinimumBias_RAW_HLTL1EG5Skim_132601_3_HLTReRunV30_V2_RECO.root",
"file:/media/disk/data10/skims/MinimumBias_RAW_HLTL1EG5Skim_132601_4_HLTReRunV30_V2_RECO.root",
"file:/media/disk/data10/skims/MinimumBias_RAW_HLTL1EG5Skim_132601_5_HLTReRunV30_V2_RECO.root",
"file:/media/disk/data10/skims/MinimumBias_RAW_HLTL1EG5Skim_132601_6_HLTReRunV30_V2_RECO.root",)

#no configuration is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");

# define path 'p': PAT Layer 0, PAT Layer 1, and the analyzer
process.p = cms.Path(#process.siPixelRecHits*
                     #process.siStripMatchedRecHits*
                     #process.firstStep*
                     #process.newCombinedSeeds*
                     #process.ecalDrivenElectronSeedsNoHE*
                    # process.generalTracks*
                    # process.trackerDrivenElectronSeeds*
                     #process.electronMergedSeedsNoHE* 
                  #   process.electronCkfTrackCandidatesNoHE*
                  #  process.electronGsfTracksNoHE*
                   #  process.gsfElectronCoresNoHE*
                   #  process.gsfElectronsNoHE*
                    # process.patDefaultSequence*
                     process.shNtupliser)
