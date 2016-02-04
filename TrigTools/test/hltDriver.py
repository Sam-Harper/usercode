import FWCore.ParameterSet.Config as cms

isData=True
outputRAW=False
maxNrEvents=3000
outputSummary=False
hltProcName="HLT3"
rmPreScales=True

from hlt import process

import SHarper.TrigTools.hltConfigTools 


process.load('Configuration/EventContent/EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
 #  outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
#outputCommands = process.RECOEventContent.outputCommands,
   
    outputCommands =cms.untracked.vstring("drop *",
                                          "keep *_TriggerResults_*_*",
                                          "keep *_hltTriggerSummaryAOD_*_*"),
                                  
    fileName = cms.untracked.string(sys.argv[len(sys.argv)-1]),
#  SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('skimP')),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('HLTDEBUG'),
  #      filterName = cms.untracked.string('JETMETFilter'),
        
                                     
    )
)
if outputRAW:
    process.output.outputCommands=cms.untracked.vstring("drop *","keep *_rawDataCollector_*_*","keep *_addPileupInfo_*_*","keep *_TriggerResults_*_*","keep *_hltTriggerSummaryAOD_*_*")
                                                                
process.HLTOutput_sam = cms.EndPath(process.output)

# limit the number of events to be processed
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( maxNrEvents )
)

# enable the TrigReport and TimeReport
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( outputSummary )
)

# override the process name
process.setName_(hltProcName)
