#shamelessly stolen from PatAnalyzerSkeleton_cfg.py

#this config file configures the PAT to be suitable for HEEP analyses and then runs an example analyzer
#this uses only bare pat and does not use the HEEP event

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
    default          = cms.untracked.PSet( limit = cms.untracked.int32(100)  ),
    PATLayer0Summary = cms.untracked.PSet( limit = cms.untracked.int32(-1) )
)


process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('IDEAL_V9::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

# this defines the input files
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))
process.PoolSource.fileNames = ['file:/scratch/sharper/cmsswDataFiles/zee_relVal_219.root']

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

#time to patify things with custom heep settings
#this reconfigures PAT to our needs, specifically turns of all pat selection except dup removal for electrons
#sets up the correct isolation
#and defines the heepPATSequence which runs the pat and our modifications
process.load("SHarper.HEEPAnalyzer.HEEPPatConfig_cfi")

#now define our bare pat analysis module
process.load("SHarper.HEEPAnalyzer.HEEPAnalyzerBarePAT_cfi")

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('output.root')
)

#very very important if you want to run on summer08 samples (it drops particle flow so unless you want to use PF, then always run this)
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import run22XonSummer08AODSIM
run22XonSummer08AODSIM(process)

process.p = cms.Path(process.heepPATSequence* #runs pat with heep settings
                     process.heepAnalyzerBarePAT) #analyses pat output


