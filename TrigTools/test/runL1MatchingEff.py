# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("PDF")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)





process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)

import sys
from SHarper.SHNtupliser.addInputFiles import addInputFiles
addInputFiles(process.source,sys.argv[2:len(sys.argv)-1])
process.effMaker = cms.EDAnalyzer("L1SeedingEffMaker",
                                  trigEventTag =cms.InputTag("hltTriggerSummaryAOD","","HLTX"),
                                  l1EGTag = cms.InputTag('hltCaloStage2Digis','EGamma' ),
                                  filterNameUnseeded= cms.string("hltEG30HEUnseededFilter"),
                                  filterNameSeeded= cms.string("hltEG30HEFilter"),
                                  minL1EGEt =cms.double(26)
                                  )
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)


isCrabJob=False #script seds this if its a crab job

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.TFileService.fileName= "OUTPUTFILE"
    process.pdfTreeMaker.datasetCode = DATASETCODE

else:
    print "using user specified filename"
    
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]

    



process.p = cms.Path(
    process.effMaker
)
 
