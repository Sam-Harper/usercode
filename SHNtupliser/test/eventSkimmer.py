
import FWCore.ParameterSet.Config as cms
process = cms.Process("Skim")

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.parseArguments()


process.source = cms.Source("PoolSource",
                        
                            fileNames = cms.untracked.vstring(options.inputFiles),
                            eventsToProcess = cms.untracked.VEventRange()
                            )
        # set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
    )





process.load('Configuration/EventContent/EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
                                  splitLevel = cms.untracked.int32(0),
                                  outputCommands = cms.untracked.vstring("drop *",
                                                                         "keep *_gedGsfElectrons_*_*",
                                                                         "keep *_gedGsfElectronCores_*_*",
                                                                         "keep *_gedPhotons_*_*",
                                                                         "keep *_gedPhotonCore_*_*",
                                                                         "keep *_particleFlowEGamma_*_*",
                                                                         "keep *_particleFlowSuperClusterECAL_*_*",
                                                                         "keep *_slimmedElectrons_*_*",
                                                                         "keep *_slimmedPhotons_*_*",
                                                                         ),
                                  fileName = cms.untracked.string(options.outputFile),
                                  
                                  dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('HLTDEBUG'),                                     
        )
                                  )


process.output_step = cms.EndPath(process.output)   
    
