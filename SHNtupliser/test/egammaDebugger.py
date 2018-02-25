# Import configurations
import FWCore.ParameterSet.Config as cms

process = cms.Process("EGDEBUG")


import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.parseArguments()

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles)
                            )                            

process.debugger = cms.EDAnalyzer("EGammaDebugger",
                                  patElectrons=cms.InputTag("slimmedElectrons"),
                                  patPhotons=cms.InputTag("slimmedPhotons"),
                                  patOOTPhotons=cms.InputTag("slimmedOOTPhotons"),
                                  )

process.p = cms.Path(process.debugger)

