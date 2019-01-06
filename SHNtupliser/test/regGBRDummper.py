
# Import configurations
import FWCore.ParameterSet.Config as cms

process = cms.Process("EGDEBUG")


import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.parseArguments()

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '102X_upgrade2018_realistic_v12', '')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles)
                            )                            

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(options.outputFile.replace(".root",process.GlobalTag.globaltag.value()+".root"))
)

process.dumper = cms.EDAnalyzer("DumpRegGBRForest",
                                names =  cms.vstring('electron_eb_ECALonly_lowpt', 'electron_eb_ECALonly', 'electron_ee_ECALonly_lowpt', 'electron_ee_ECALonly','electron_eb_ECALonly_lowpt_var', 'electron_eb_ECALonly_var', 'electron_ee_ECALonly_lowpt_var', 'electron_ee_ECALonly_var','electron_eb_ECALTRK_lowpt', 'electron_eb_ECALTRK', 'electron_ee_ECALTRK_lowpt', 'electron_ee_ECALTRK','electron_eb_ECALTRK_lowpt_var', 'electron_eb_ECALTRK_var', 'electron_ee_ECALTRK_lowpt_var', 'electron_ee_ECALTRK_var','photon_eb_ECALonly_lowpt', 'photon_eb_ECALonly', 'photon_ee_ECALonly_lowpt', 'photon_ee_ECALonly','photon_eb_ECALonly_lowpt_var', 'photon_eb_ECALonly_var', 'photon_ee_ECALonly_lowpt_var', 'photon_ee_ECALonly_var')
                                )


process.p = cms.Path(process.dumper)

