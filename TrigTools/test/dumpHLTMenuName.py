import FWCore.ParameterSet.Config as cms

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis')
options.register('hltProcess',"HLT",options.multiplicity.singleton,options.varType.string,"HLT process name")
options.register('paths',"",options.multiplicity.list,options.varType.string,"paths to print")
options.parseArguments()

# set up process
process = cms.Process("TrigExample")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkSummary = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500000),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500000),
    limit = cms.untracked.int32(10000000)
)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles)
)

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.hltDumper =  cms.EDAnalyzer("DumpHLTMenuName",
                                    hltProcess=cms.string(options.hltProcess),
                                    pathsToPrintFilters = cms.vstring(options.paths)
                                )


process.p = cms.Path(process.hltDumper)
