isMC=True

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(10),
    limit = cms.untracked.int32(10000000)
)


process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
if isMC:
    process.GlobalTag.globaltag = autoCond['startup'] 
else:
    process.GlobalTag.globaltag = autoCond['com10']

process.load("Configuration.StandardSequences.MagneticField_cff")


# this defines the input files
import sys
filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])




# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# input heep analyzer sequence
process.load("SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi")


process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)
#this is a safety to stop the unwary deleteing their input file ;)
if len(sys.argv)>2:
    process.TFileService.fileName = cms.string(sys.argv[len(sys.argv)-1])



#for isolation correction
from RecoJets.JetProducers.kt4PFJets_cfi import kt4PFJets
process.kt6PFJetsForIsolation = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True )
process.kt6PFJetsForIsolation.Rho_EtaMax = cms.double(2.5)


process.p = cms.Path(process.kt6PFJetsForIsolation*process.heepAnalyzer) #runs heep analyzer


