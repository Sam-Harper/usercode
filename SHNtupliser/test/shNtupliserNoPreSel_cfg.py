#shamelessly stolen from PatAnalyzerSkeleton_cfg.py

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEPAnalyzer")

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
process.MessageLogger.suppressWarning = cms.untracked.vstring("selectedLayer1Hemispheres")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('PATLayer0Summary')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet( limit = cms.untracked.int32(0)  ),
    PATLayer0Summary = cms.untracked.PSet( limit = cms.untracked.int32(-1) )
)



process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('IDEAL_V9::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

#process.load("Geometry.CaloEventSetup.CaloGeometry_cfi")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")



process.load("RecoLocalTracker.SiPixelRecHits.SiPixelRecHits_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitConverter_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitMatcher_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.StripCPEfromTrackAngle_cfi")
process.load("RecoLocalTracker.SiStripZeroSuppression.SiStripZeroSuppression_cfi")
process.load("RecoLocalTracker.SiStripClusterizer.SiStripClusterizer_cfi")
process.load("RecoLocalTracker.SiPixelClusterizer.SiPixelClusterizer_cfi")
process.load("RecoLocalTracker.SiPixelRecHits.PixelCPEESProducers_cff")
process.load("RecoTracker.TransientTrackingRecHit.TTRHBuilders_cff")


# this defines the input files
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))
process.PoolSource.fileNames = ['file:/scratch/sharper/cmsswDataFiles/zee_relVal_219.root']

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.load("RecoEgamma.EgammaElectronProducers.pixelMatchGsfElectronSequenceNoCuts_cff")
#time to patify things with custom heep settings
process.load("SHarper.HEEPAnalyzer.HEEPPatConfigNoPreSel_cfi")

process.load("SHarper.SHNtupliser.shNtupliser_cfi")

process.shNtupliser.datasetCode = 2
process.shNtupliser.sampleWeight = .1


process.shNtupliser.outputFilename="relVal_Zee_219.root"


from PhysicsTools.PatAlgos.tools.cmsswVersionTools import run22XonSummer08AODSIM
run22XonSummer08AODSIM(process)


# define path 'p': PAT Layer 0, PAT Layer 1, and the analyzer
process.p = cms.Path(process.siPixelRecHits*process.siStripMatchedRecHits*process.newSeedFromPairs*process.newSeedFromTriplets*process.newCombinedSeeds*
    process.pixelMatchGsfElectronSequenceNoCuts*
    process.heepPATSequence*
    process.shNtupliser)


