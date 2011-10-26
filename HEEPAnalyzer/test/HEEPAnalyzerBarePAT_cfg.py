#shamelessly stolen from PatAnalyzerSkeleton_cfg.py

#this config file configures the PAT to be suitable for HEEP analyses and then runs an example analyzer
#this uses only bare pat and does not use the HEEP event

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

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('START42_V15B::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

# this defines the input files
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))
process.PoolSource.fileNames = ['file:/media/usbdisk1/zee_relVal_312_F0303A91-9278-DE11-AADC-001D09F25456.root']

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

#no configuration of the pat is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");

#now define our bare pat analysis module
process.load("SHarper.HEEPAnalyzer.HEEPAnalyzerBarePAT_cfi")

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('output.root')
)


process.p = cms.Path(process.patDefaultSequence* #runs PAT 
                     process.heepAnalyzerBarePAT) #runs heep analyzer
