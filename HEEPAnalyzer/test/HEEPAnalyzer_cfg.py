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
    input = cms.untracked.int32(1000)
)

#time to patify things with custom heep settings
#this reconfigures PAT to our needs, specifically turns of all pat selection except dup removal for electrons
#sets up the correct isolation
#and defines the heepPATSequence which runs the pat and our modifications
process.load("SHarper.HEEPAnalyzer.HEEPPatConfig_cfi");

# input heep analyzer sequence
process.load("SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi")

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('output.root')
)

#run the heep analyzer
process.p = cms.Path(process.heepPATSequence*
                     process.heepAnalyzer)


