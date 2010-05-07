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
process.GlobalTag.globaltag = cms.string('MC_3XY_V26::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

#to allow me to re-run the tracking to redo electron seeds without H/E



process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = DATASETCODE
process.shNtupliser.sampleWeight = SAMPLEWEIGHT
process.shNtupliser.outputFilename="OUTPUTFILE"
process.shNtupliser.minEtToPromoteSC = 999999

process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))

#no configuration is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");

# define path 'p': PAT Layer 0, PAT Layer 1, and the analyzer
process.p = cms.Path(
                     process.patDefaultSequence*
                     process.shNtupliser)

from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
run33xOn31xMC( process,
                    jetSrc = cms.InputTag("antikt5CaloJets"),
                    jetIdTag = "antikt5"
                    )
##process.jetCorrFactors.corrSample = 'Summer09_7TeV'
