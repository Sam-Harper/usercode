
# Import configurations
import FWCore.ParameterSet.Config as cms


process = cms.Process("Combine")


process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
#                            inputCommands=cms.untracked.vstring('keep *',
#                                                                'drop *_hltEgammaPixelSeedVars*_*_*')
                        #    eventsToProcess = cms.untracked.VEventRange("1:1484800-1:1484810"),
#                            eventsToSkip = cms.untracked.VEventRange("1:1484806-1:1484806")
                             )
import sys
from SHarper.SHNtupliser.addInputFiles import addInputFiles
addInputFiles(process.source,sys.argv[2:len(sys.argv)-1])

process.output = cms.OutputModule( "PoolOutputModule",
                                   fileName = cms.untracked.string(sys.argv[-1]),
                                   fastCloning = cms.untracked.bool( False ),
                                   dataset = cms.untracked.PSet(
        filterName = cms.untracked.string( "" ),
        dataTier = cms.untracked.string( "RAW" )
        ),
                                   
                                   outputCommands = cms.untracked.vstring( 'keep *')
)

process.outPath = cms.EndPath(process.output)
