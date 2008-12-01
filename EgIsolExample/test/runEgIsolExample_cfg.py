# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process 
process = cms.Process("EgIsolExample")

# initialize MessageLogger and output report (this is purely to reduce the amount of stuff spammed to my screen)
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkSummary = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(100),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(100),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet( limit = cms.untracked.int32(0)  )
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )
#end intialising the MessageLogger

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('IDEAL_V9::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

# this defines the input files
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring()
)
process.PoolSource.fileNames = ['file:/scratch/sharper/cmsswDataFiles/zee_relVal_219.root']

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)


#define the main isolation sequence which produces ecal, hcal, track
#it is called PAT as it uses IsoDeposits which is what PAT uses
process.load("RecoEgamma.EgammaIsolationAlgos.egammaIsolationSequencePAT_cff")
#define our additional isolation sequences which produce hcal depth1, hcal depth2
process.load("SHarper.EgIsolExample.eleHcalDepthSplitIsolSequence_cff")

#configure our example module
process.load("SHarper.EgIsolExample.EgIsolExample_cfi");


#setup our output file
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('output.root')
)

#run the test analyzer
process.p = cms.Path(process.egammaIsolationSequencePAT*process.eleHcalDepthSplitIsolSequence*process.egIsolExample)


