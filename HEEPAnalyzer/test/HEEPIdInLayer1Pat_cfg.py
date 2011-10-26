#this config places the HEEPId into allLayer1Electrons

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
process.PoolSource.fileNames = [
        '/store/relval/CMSSW_3_3_6/RelValZEE/GEN-SIM-RECO/MC_3XY_V9A-v1/0009/FE31828E-3BE4-DE11-AB0D-002618FDA207.root',
        '/store/relval/CMSSW_3_3_6/RelValZEE/GEN-SIM-RECO/MC_3XY_V9A-v1/0009/D6493FA6-9EE4-DE11-8495-002618943856.root',
        '/store/relval/CMSSW_3_3_6/RelValZEE/GEN-SIM-RECO/MC_3XY_V9A-v1/0009/6A5634A0-3CE4-DE11-ADE3-002354EF3BDC.root',
        '/store/relval/CMSSW_3_3_6/RelValZEE/GEN-SIM-RECO/MC_3XY_V9A-v1/0009/36B31AA0-3CE4-DE11-9D19-00304867920C.root',
        '/store/relval/CMSSW_3_3_6/RelValZEE/GEN-SIM-RECO/MC_3XY_V9A-v1/0008/545EBB92-3AE4-DE11-AFA0-003048679296.root']

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

#setting up the producer to make the HEEP ID value map
from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *
process.HEEPId = cms.EDProducer("HEEPIdValueMapProducer",
                                eleLabel = cms.InputTag("gsfElectrons"),
                                barrelCuts = cms.PSet(heepBarrelCuts),
                                endcapCuts = cms.PSet(heepEndcapCuts)
                                )
process.HEEPId.barrelCuts.minEt = 5. #reset min et cut to 5 GeV
process.HEEPId.endcapCuts.minEt = 5. #reset min et cut to 5 GeV

#configure the pat to load the id in
process.load("PhysicsTools.PatAlgos.patSequences_cff");
process.patElectrons.userData.userInts.src = cms.VInputTag('HEEPId')


process.p = cms.Path(process.HEEPId* #makes the HEEPID value map
                     process.patDefaultSequence) #runs PAT
