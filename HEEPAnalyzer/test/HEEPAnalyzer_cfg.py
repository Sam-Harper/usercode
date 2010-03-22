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
process.GlobalTag.globaltag = cms.string('MC_3XY_V25::All')
process.load("Configuration.StandardSequences.MagneticField_cff")


# this defines the input files
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))
process.PoolSource.fileNames = [  '/store/relval/CMSSW_3_5_4/RelValZEE/GEN-SIM-RECO/MC_3XY_V24-v1/0004/70B2BC04-2D2C-DF11-A5EC-002618FDA28E.root',
       '/store/relval/CMSSW_3_5_4/RelValZEE/GEN-SIM-RECO/MC_3XY_V24-v1/0003/9C5D2907-832B-DF11-9E86-0018F3D09628.root',
       '/store/relval/CMSSW_3_5_4/RelValZEE/GEN-SIM-RECO/MC_3XY_V24-v1/0003/62813863-832B-DF11-BB58-001A928116B8.root',
       '/store/relval/CMSSW_3_5_4/RelValZEE/GEN-SIM-RECO/MC_3XY_V24-v1/0003/582519E2-842B-DF11-BCD2-002618943954.root',
       '/store/relval/CMSSW_3_5_4/RelValZEE/GEN-SIM-RECO/MC_3XY_V24-v1/0003/026E24AA-7D2B-DF11-BD37-0018F3D096AA.root'
  ]
#process.PoolSource.fileNames = ['file:/media/usbdisk1/zee_relVal_312_F0303A91-9278-DE11-AADC-001D09F25456.root']

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

#no configuration is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");
# input heep analyzer sequence
process.load("SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi")

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('heepEventOutput.root')
)


process.p = cms.Path(process.patDefaultSequence* #runs PAT 
                     process.heepAnalyzer) #runs heep analyzer


