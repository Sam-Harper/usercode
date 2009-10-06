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
process.GlobalTag.globaltag = cms.string('MC_31X_V3::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

# this defines the input files
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring('dummy'))
process.PoolSource.fileNames = [
    '/store/relval/CMSSW_3_2_6/RelValZEE/GEN-SIM-RECO/STARTUP31X_V7-v1/0013/9AB61A32-BA9A-DE11-8298-001D09F29619.root',
    '/store/relval/CMSSW_3_2_6/RelValZEE/GEN-SIM-RECO/STARTUP31X_V7-v1/0013/80CF3175-C59A-DE11-AB6E-0016177CA778.root',
    '/store/relval/CMSSW_3_2_6/RelValZEE/GEN-SIM-RECO/STARTUP31X_V7-v1/0013/7047D992-FC9A-DE11-863B-0019B9F72BAA.root',
    '/store/relval/CMSSW_3_2_6/RelValZEE/GEN-SIM-RECO/STARTUP31X_V7-v1/0013/5CC96C97-B59A-DE11-8CAA-0019B9F6C674.root',
    '/store/relval/CMSSW_3_2_6/RelValZEE/GEN-SIM-RECO/STARTUP31X_V7-v1/0013/126AA31F-BE9A-DE11-97E9-001D09F290BF.root']

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

#no configuration of the pat is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");

from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *
process.heepPatElectrons = cms.EDProducer("HEEPAttStatusToPAT",
                                          eleLabel = cms.InputTag("allLayer1Electrons"),
                                          barrelCuts = cms.PSet(heepBarrelCuts),
                                          endcapCuts = cms.PSet(heepEndcapCuts)
                                          )

process.heepAnalyzerHEEPPAT = cms.EDProducer("HEEPAnalyzerHEEPPAT",
                                             eleLabel = cms.InputTag("heepPatElectrons"),
                                             )


process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('heepPATOutput.root')
)


process.p = cms.Path(process.patDefaultSequence* #runs PAT 
                     process.heepPatElectrons* #heepifies the pat electrons (resets energy to ecal energy and adds heep id)
                     process.heepAnalyzerHEEPPAT) #runs heep analyzer

#if you wanted to write out the patElectrons, uncomment the following

##process.load("Configuration.EventContent.EventContent_cff")
##process.out = cms.OutputModule("PoolOutputModule",
##                               process.FEVTSIMEventContent,
##                               fileName = cms.untracked.string('file:test.root')
##)
##process.out.outputCommands.append('drop *_*_*_*')
##process.out.outputCommands.append('keep patElectrons_*_*_*')
##process.outpath = cms.EndPath(process.out)
