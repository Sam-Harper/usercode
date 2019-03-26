import FWCore.ParameterSet.Config as cms

process = cms.Process('MCSkim')

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.parseArguments()

process.options = cms.untracked.PSet()
process.options.numberOfThreads=cms.untracked.uint32(2)
process.options.numberOfStreams=cms.untracked.uint32(2)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles),
    secondaryFileNames = cms.untracked.vstring()
)

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(100),
    limit = cms.untracked.int32(10000000)
)


process.mcPtFilter = cms.EDFilter("MCPtFilter",
                                  genParts = cms.InputTag("genParticles"),
                                  pid=cms.int32(11),
                                  minPt = cms.double(0.),
                                  maxPt = cms.double(30.)
)


process.filterPath = cms.Path(process.mcPtFilter)

process.output = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(4),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('AODSIM'),
        filterName = cms.untracked.string('')
    ),
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('filterPath')),
    fileName = cms.untracked.string(options.outputFile.replace(".root","_EDM.root")),
    outputCommands = cms.untracked.vstring('keep *')
)
process.outPath = cms.EndPath(process.output)
