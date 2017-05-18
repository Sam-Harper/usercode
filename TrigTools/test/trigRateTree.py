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
    addInputFiles(process.source,sys.argv[2:len(sys.argv)-1])
    from SHarper.SHNtupliser.datasetCodes import getDatasetCode
    datasetCode=getDatasetCode(process.source.fileNames[0])

if datasetCode==0: isMC=False
else: isMC=True

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
    process.GlobalTag.globaltag = autoCond['run2_data']


# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("Configuration.StandardSequences.Services_cff")


process.trigRateTree = cms.EDAnalyzer("TrigRateTree",
                                      nrEventsInRun=cms.InputTag("nrEventsStorer","nrEventsRun","HLTSkim"),
                                      nrEventsInRunPostPUFilter=cms.InputTag("nrEventsStorerPostPUFilter","nrEventsRun","HLTSkim"),
                                      triggerResults=cms.InputTag("TriggerResults","","HLTX"),
                                      triggerResultsPUFilter=cms.InputTag("TriggerResults","","HLTSkim"),
                                      pileupInfo = cms.InputTag("addPileupInfo"),
                                      pileupFilterPath = cms.string("HLT_RemovePileUpDominatedEventsGen_v1"),
                                      triggerPaths= cms.vstring("HLT_DoubleEle5_CaloIdL_MW_v8","HLT_DoubleEle33_CaloIdL_MW_v8","HLT_DoublePhoton33_CaloIdL_v1","HLT_Photon33_v1","HLT_Photon50_v7","HLT_Photon75_v7","HLT_Photon90_v7","HLT_Photon120_v7","HLT_Photon150_v1","HLT_Photon175_v8","HLT_Photon200_v8","HLT_ECALHT800_v6","HLT_CaloJet500_NoJetID_v5","HLT_CaloJet550_NoJetID_v1","HLT_Photon300_NoHE_v7","HLT_DoublePhoton70_v1","HLT_DoublePhoton85_v8","HLT_DiSC30_18_EIso_AND_HE_Mass70_v6","HLT_Ele23_Ele12_CaloIdL_CaloIso_v1","HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v9","HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v9","HLT_Ele27_WPTight_Gsf_NoPM_v1","HLT_Ele27_WPTight_Gsf_v7","HLT_Ele32_WPTight_Gsf_v1","HLT_Ele35_WPTight_Gsf_v1","HLT_Ele38_WPTight_Gsf_v1","HLT_Ele38_WPTightV2_Gsf_v1","HLT_Ele38_WPTight_Gsf_MW_v1","HLT_Ele40_WPTight_Gsf_v1"),
                                      datasetCode=cms.int32(datasetCode)
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
    process.trigRateTree.datasetCode = datasetCode

else:
    print "using user specified filename"
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]
    process.trigRateTree.datasetCode = datasetCode



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
