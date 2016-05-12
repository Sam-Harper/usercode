
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
                        #    eventsToProcess = cms.untracked.VEventRange("1:1484800-1:1484810"),
#                            eventsToSkip = cms.untracked.VEventRange("1:1484806-1:1484806")
                             )


import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis')
options.register ('hltPaths',
                  "HLT_*",
                  VarParsing.VarParsing.multiplicity.list,
                  VarParsing.VarParsing.varType.string,          
                  "HLT paths to select")

options.parseArguments()


from SHarper.SHNtupliser.addInputFiles import addInputFiles
addInputFiles(process.source,options.inputFiles)
    

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(10000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)




import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.HLTPaths = cms.vstring(options.hltPaths)

process.skimPath = cms.Path(process.skimHLTFilter)

process.load('Configuration/EventContent/EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
#  outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
 # outputCommands = process.AODSIMEventContent.outputCommands,
    outputCommands = cms.untracked.vstring("keep *"),
    fileName = cms.untracked.string(options.outputFile),
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('skimPath')),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('HLTDEBUG'),                                     
    )
)
process.outPath = cms.EndPath(process.output)
