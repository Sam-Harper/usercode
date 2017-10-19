def getDatasetCode(filename):
    if filename.find("HLTPhysics1")!=-1: return 1
    if filename.find("HLTPhysics2")!=-1: return 2
    if filename.find("HLTPhysics3")!=-1: return 3
    if filename.find("HLTPhysics4")!=-1: return 4
    if filename.find("HLTPhysics5")!=-1: return 5
    if filename.find("HLTPhysics6")!=-1: return 6
    if filename.find("HLTPhysics7")!=-1: return 7
    if filename.find("HLTPhysics8")!=-1: return 8
    return 0

isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
                        #    eventsToProcess = cms.untracked.VEventRange("1:1484800-1:1484810"),
#                            eventsToSkip = cms.untracked.VEventRange("1:1484806-1:1484806")
                             )
if isCrabJob:
    datasetCode=DATASETCODE
else:
    import sys
    from SHarper.SHNtupliser.addInputFiles import addInputFiles
    addInputFiles(process.source,sys.argv[2:-1])
    datasetCode=getDatasetCode(process.source.fileNames[0])


isMC=True

print "isCrab = ",isCrabJob,"isMC = ",isMC," datasetCode = ",datasetCode

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(10000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

#Load geometry
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.autoCond import autoCond
if isMC:
    process.GlobalTag.globaltag = autoCond['run2_mc'] 
else:
    process.GlobalTag.globaltag = "92X_dataRun2_Prompt_v9"


# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("Configuration.StandardSequences.Services_cff")


process.trigRateTree = cms.EDAnalyzer("TrigResultTreeMakerV3",
                                      trigResultsTag=cms.InputTag("TriggerResults","","HLTX"),
                                      trigResultsP5Tag=cms.InputTag("TriggerResults","","HLT"),
                                      datasetCode = cms.int32(datasetCode)
                                      )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)
#process.shNtupliser.gsfEleTag = cms.InputTag("gedGsfElectronsTrkIsoCorr")
import os


#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.TFileService.fileName= "OUTPUTFILE"


else:
    print "using user specified filename"
    process.TFileService.fileName= sys.argv[-1]




#import HLTrigger.HLTfilters.hltHighLevel_cfi
#process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
#process.skimHLTFilter.throw=cms.bool(False)
#process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLTSkim")
#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_RemovePileUpDominatedEventsGen_v1",)


process.p = cms.Path(process.trigRateTree)

#from SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi import swapHEEPToMiniAOD
#swapHEEPToMiniAOD(process.shNtupliser)
#process.source.eventsToProcess = cms.untracked.VEventRange("1:57017-1:57017",)
#from SHarper.HEEPAnalyzer.heepTools import *
#swapCollection(process,"gsfElectrons","gsfElectronsHEEPCorr")
print process.GlobalTag.globaltag
