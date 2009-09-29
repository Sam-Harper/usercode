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
#process.MessageLogger.suppressWarning = cms.untracked.vstring("selectedLayer1Hemispheres")
#process.MessageLogger.cerr.threshold = 'INFO'
#process.MessageLogger.categories.append('PATLayer0Summary')
#process.MessageLogger.cerr.INFO = cms.untracked.PSet(
#    default          = cms.untracked.PSet( limit = cms.untracked.int32(100)  ),
#    PATLayer0Summary = cms.untracked.PSet( limit = cms.untracked.int32(-1) )
#)


process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('MC_31X_V3::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

# this defines the input files
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))
process.PoolSource.fileNames = ['file:/media/usbdisk1/zee_relVal_312_F0303A91-9278-DE11-AADC-001D09F25456.root']

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

#no configuration is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");
# input heep analyzer sequence
process.load("SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi")

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('output.root')
)


process.p = cms.Path(process.patDefaultSequence* #runs PAT 
                     process.heepAnalyzer) #runs heep analyzer


