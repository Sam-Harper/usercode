# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("PhoSkim")

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
                        #  fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])


# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Photon30_CaloIdVL_v*","HLT_Photon50_CaloIdVL_v*","HLT_Photon75_CaloIdVL_v*","HLT_Photon90_CaloIdVL_v*","HLT_Photon135_v*","HLT_Photon150_v*")
process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.singlePhoFilter = cms.EDFilter("EventListSkimmer",
                                       filename = cms.string("SHarper/TrigTools/data/singlePhotonNrVertSkimSorted.list")
                                       )
process.skimP = cms.Path(process.skimHLTFilter*process.singlePhoFilter)    
process.load('Configuration/EventContent/EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
   #outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
#outputCommands = process.RECOEventContent.outputCommands,
    outputCommands = cms.untracked.vstring("keep *"),
  fileName = cms.untracked.string("dummy.root"),
  SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('skimP')),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('HLTDEBUG'),
  #      filterName = cms.untracked.string('JETMETFilter'),
        
                                     
    )
)

isCrabJob=False

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.output.fileName= "OUTPUTFILE"
    
else:
    print "using user specified filename"
    process.output.fileName= sys.argv[len(sys.argv)-1]


process.HLTOutput_sam = cms.EndPath(process.output)
