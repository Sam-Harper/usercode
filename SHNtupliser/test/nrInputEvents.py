
# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
                        #    eventsToProcess = cms.untracked.VEventRange("1:1484800-1:1484810"),
#                            eventsToSkip = cms.untracked.VEventRange("1:1484806-1:1484806")
                             )

import sys
from SHarper.SHNtupliser.addInputFiles import addInputFiles
addInputFiles(process.source,sys.argv[2:len(sys.argv)])
   

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(10000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.nrEventsReader=cms.EDAnalyzer("NrInputEventsReader",
                                      eventsLumi=cms.InputTag("nrEventsStorer","nrEventsLumi"),
                                      eventsRun=cms.InputTag("nrEventsStorer","nrEventsRun"),
)
process.nrEventsReaderPostPU=cms.EDAnalyzer("NrInputEventsReader",
                                      eventsLumi=cms.InputTag("nrEventsStorerPostPUFilter","nrEventsLumi"),
                                      eventsRun=cms.InputTag("nrEventsStorerPostPUFilter","nrEventsRun"),
)

process.p = cms.Path(process.nrEventsReader*process.nrEventsReaderPostPU)
