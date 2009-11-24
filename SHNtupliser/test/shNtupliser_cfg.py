#shamelessly stolen from PatAnalyzerSkeleton_cfg.py

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('MC_31X_V3::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
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

process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = 0
process.shNtupliser.sampleWeight = 1.0
process.shNtupliser.outputFilename="zee_Summer09_ntuples_v12_1.root"

process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))
process.source.fileNames = [
    "file:///media/usbdisk1/Oct09/Photon20/Zee/F2989327-DDAB-DE11-97C2-003048679274.root",
    "file:///media/usbdisk1/Oct09/Photon20/Zee/30854AF0-F6AB-DE11-AEB7-003048678B16.root",
    "file:///media/usbdisk1/Oct09/Photon20/Zee/7C78679A-F5AB-DE11-A55A-001A92810AEA.root",
    "file:///media/usbdisk1/Oct09/Photon20/Zee/BA857AE9-74AC-DE11-8062-0018F3D09648.root",
]

##process.load("SHarper.HEEPAnalyzer.RelValZee_312_cfi")
#no configuration is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");
process.allLayer1Electrons.electronSource="gsfElectronsNoHE"

process.shNtupliser.gsfEleTag = "gsfElectronsNoHE"

# define path 'p': PAT Layer 0, PAT Layer 1, and the analyzer
process.p = cms.Path(process.siPixelRecHits*
                     process.siStripMatchedRecHits*
                     process.firstStep*
                     process.newCombinedSeeds*
                     process.ecalDrivenElectronSeedsNoHE*
                    # process.generalTracks*
                    # process.trackerDrivenElectronSeeds*
                     #process.electronMergedSeedsNoHE* 
                     process.electronCkfTrackCandidatesNoHE*
                     process.electronGsfTracksNoHE*
                     process.gsfElectronCoresNoHE*
                     process.gsfElectronsNoHE*
                    # process.patDefaultSequence*
                     process.shNtupliser)


