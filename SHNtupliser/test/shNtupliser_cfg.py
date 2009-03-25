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
# this defines the input files
#process.load("SHarper.HEEPAnalyzer.qcd_20_30_RelVal_212_cfi");
#process.load("SHarper.SHNtupliser.relVal_QCD_Pt_80_120_218_cfi");

#process.load("RecoLocalTracker.SiPixelRecHits.SiPixelRecHits_cfi")
#process.load("RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitConverter_cfi")
#process.load("RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitMatcher_cfi")
#process.load("RecoLocalTracker.SiStripRecHitConverter.StripCPEfromTrackAngle_cfi")
#process.load("RecoLocalTracker.SiStripZeroSuppression.SiStripZeroSuppression_cfi")
#process.load("RecoLocalTracker.SiStripClusterizer.SiStripClusterizer_cfi")
#process.load("RecoLocalTracker.SiPixelClusterizer.SiPixelClusterizer_cfi")
#process.load("RecoLocalTracker.SiPixelRecHits.PixelCPEESProducers_cff")
#process.load("RecoTracker.TransientTrackingRecHit.TTRHBuilders_cff")


# this defines the input files
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))
process.PoolSource.fileNames = ['file:/scratch/sharper/cmsswDataFiles/zee_relVal_219.root']

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

#process.load("RecoEgamma.EgammaElectronProducers.pixelMatchGsfElectronsNoCuts_cfi")
#process.load("RecoEgamma.EgammaElectronProducers.pixelMatchGsfElectronSequenceNoCuts_cff")
# input pat sequences
#from PhysicsTools.PatAlgos.patLayer0_cff import *
#from PhysicsTools.PatAlgos.patLayer1_cff import *
#time to patify things with custom heep settings
process.load("SHarper.HEEPAnalyzer.HEEPPatConfig_cfi");

# input heep analyzer sequence
#process.load("SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi")
process.load("SHarper.SHNtupliser.shNtupliser_cfi")
#process.shNtupliser.barrelRecHitTag = cms.InputTag("caloRecHits","EcalRecHitsEB")
#process.shNtupliser.endcapRecHitTag = cms.InputTag("caloRecHits","EcalRecHitsEE")
process.shNtupliser.datasetCode = 2
process.shNtupliser.sampleWeight = .1
#process.eleIsoDepositEcalFromHits.barrelEcalHits =  cms.InputTag("caloRecHits","EcalRecHitsEB")
#process.eleIsoDepositEcalFromHits.endcapEcalHits =  cms.InputTag("caloRecHits","EcalRecHitsEE")
#process.shNtupliser.hbheRecHitsTag =  cms.InputTag("caloRecHits")
#process.load("SHarper.SHNtupliser.relVal_Zee_220_full_cfi");
#process.PoolSource.fileNames = ['file:/scratch/sharper/cmsswDataFiles/zee_relVal_219.root']
#process.PoolSource.fileNames = ['file:/media/usbdisk1/fastSimFiles/QCD_Pt_80_120_cfi_GEN_FASTSIM_IDEAL_0.root']

process.shNtupliser.outputFilename="relVal_Zee_219.root"
#process.TFileService = cms.Service("TFileService",
#                                   fileName = cms.string('output.root')
#)
#process.PoolSource.fileNames = ['file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_0.root',
#                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_1.root',
#                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_10.root',
#                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_2.root',
#                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_3.root',
 #                               'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_4.root',
  #                              'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_5.root',
   #                             'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_6.root',
    #                            'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_7.root',
     #                           'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_8.root',
      #                          'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_9.root']

from PhysicsTools.PatAlgos.tools.cmsswVersionTools import run22XonSummer08AODSIM
run22XonSummer08AODSIM(process)


# define path 'p': PAT Layer 0, PAT Layer 1, and the analyzer
process.p = cms.Path(#process.siPixelRecHits*process.siStripMatchedRecHits*process.newSeedFromPairs*process.newSeedFromTriplets*process.newCombinedSeeds*
                     #process.patLayer1*
#                     process.eleIsoDepositEcalFromHitsFast*
#                     process.eleIsoDepositEcalFromHits*
#                      process.eleIsoDepositHcalFromHits*
    #process.pixelMatchGsfElectronSequenceNoCuts*
    #                 process.pixelMatchGsfElectronsNoCuts*
                     process.heepPATSequence*
                     process.shNtupliser)


# load the pat layer 1 event content
#process.load("PhysicsTools.PatAlgos.patLayer1_EventContent_cff")

# setup event content: drop everything before PAT
#process.patEventContent = cms.PSet(
#    outputCommands = cms.untracked.vstring('drop *')
#)

# extend event content to include PAT objects
#process.patEventContent.outputCommands.extend(process.patLayer1EventContent.outputCommands)


# talk to output module
#process.out = cms.OutputModule("PoolOutputModule",
#    process.patEventSelection,
#    process.patEventContent,
#    verbose = cms.untracked.bool(False),
#    fileName = cms.untracked.string('PatAnalyzerSkeletonSkim.root')
#)

# define output path
#process.outpath = cms.EndPath(process.out)


