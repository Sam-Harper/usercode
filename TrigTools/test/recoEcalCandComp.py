# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("TrigComp")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkSummary = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)


import sys
filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

if(sys.argv[2].find("/castor/")==0):
    filePrefex="rfio:"
process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])


# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.trigComp = cms.EDAnalyzer("RecoEcalCandidateComparer",           
                                  trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
                                  trigColl1=cms.string("hltEle33CaloIdLGsfTrkIdVLDPhiFilter"),
                                  trigColl2=cms.string("hltDiEle33CaloIdLGsfTrkIdVLDPhiUnseededFilter"),
                                  collection1=cms.InputTag("hltEgammaCandidates"),
                                  collection2=cms.InputTag("hltEgammaCandidatesUnseeded")
                                  )



process.p = cms.Path(process.trigComp)
