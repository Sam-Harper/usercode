# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("TrigMass")
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
                        #    eventsToProcess = cms.untracked.VEventRange("1:1484800-1:1484810"),
#                            eventsToSkip = cms.untracked.VEventRange("1:1484806-1:1484806")
                             )
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
from SHarper.SHNtupliser.addInputFiles import addInputFiles
addInputFiles(process.source,sys.argv[2:len(sys.argv)-1])

# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")
process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.hltZPeak = cms.EDAnalyzer("HLTZPeakMon",                                  
                                  trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
                                  filterName = cms.string("hltEle23Ele12CaloIdLTrackIdLIsoVLDZFilter"),
                                  treeName = cms.string("hltDiEleMass")
)
                                  
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("test.root")
)

isCrabJob = False
#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.TFileService.fileName= "OUTPUTFILE"
else:
    print "using user specified filename"
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]
        

process.p = cms.Path(process.skimHLTFilter*                  
                     process.hltZPeak)

##import PhysicsTools.PythonAnalysis.LumiList as LumiList
##import FWCore.ParameterSet.Types as CfgTypes
##myLumis = LumiList.LumiList(filename = 'goodList.json').getCMSSWString().split(',')
##process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
##process.source.lumisToProcess.extend(myLumis)
