#shamelessly stolen from PatAnalyzerSkeleton_cfg.py

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEPAnalyzer")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('PATLayer0Summary')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet( limit = cms.untracked.int32(0)  ),
    PATLayer0Summary = cms.untracked.PSet( limit = cms.untracked.int32(-1) )
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('IDEAL_V9::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

# this defines the input files
process.load("SHarper.HEEPAnalyzer.zee_RelVal_214_cfi");


# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(200)
)


# input pat sequences
process.load("PhysicsTools.PatAlgos.patLayer0_cff")
process.load("PhysicsTools.PatAlgos.patLayer1_cff")

# input heep analyzer sequence
process.load("SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi")

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('output.root')
)

# define path 'p': PAT Layer 0, PAT Layer 1, and the analyzer
process.p = cms.Path(process.patLayer0*
                     process.patLayer1*
                     process.heepAnalyzer)


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


