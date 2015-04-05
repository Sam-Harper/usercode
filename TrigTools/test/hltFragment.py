
from SHarper.SHNtupliser.setSaveTags import setSaveTags
setSaveTags(process,"HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v1",True)
setSaveTags(process,"HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v1",True)
from SHarper.SHNtupliser.rmPaths import rmPaths
rmPaths(process,["HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v1","HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v1"])
import sys
process.source.fileNames = cms.untracked.vstring()

isCrabJob=False #script seds this if its a crab job

#if 1, its a crab job...
if isCrabJob:
    process.hltOutputA.fileName=cms.untracked.string("OUTPUTFILE")
else:
    from SHarper.SHNtupliser.addInputFiles import addInputFiles
    addInputFiles(process.source,sys.argv[2:len(sys.argv)-1])
    process.hltOutputA.fileName = cms.untracked.string(sys.argv[len(sys.argv)-1] )
    
process.hltOutputA.SelectEvents = cms.untracked.PSet()
process.load('Configuration.EventContent.EventContent_cff')
process.hltOutputA.outputCommands =  process.AODSIMEventContent.outputCommands
process.options.wantSummary =cms.untracked.bool(True)
process.maxEvents.input=cms.untracked.int32(-1)
