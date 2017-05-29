isCrabJob=False #script seds this if its a crab job


def getNrEvents(datasetCode):
    if datasetCode == 602: return  14381090
    if datasetCode == 603: return  16687875
    if datasetCode == 604: return  5238732
    if datasetCode == 605: return  2062191
    if datasetCode == 606: return  480374
    if datasetCode == 607: return  389480
    if datasetCode == 608: return  401312
    return 1

def getCrossSec(datasetCode):

  if datasetCode==602: return 140932000. #QCD_Pt-30to50
  if datasetCode==603: return 19204300. #QCD_Pt-50to80
  if datasetCode==604: return 2762530. #QCD_Pt-80to120
  if datasetCode==605: return 471100. #QCD_Pt-120to170  
  if datasetCode==606: return 117276. #QCD_Pt-170to300
  if datasetCode==607: return 7823. #QCD_Pt-300to470
  if datasetCode==608: return 648.2 #QCD_Pt-470to600
  return 0

def getWeight(datasetCode):
    print datasetCode, getCrossSec(datasetCode),getNrEvents(datasetCode)
    return getCrossSec(datasetCode)/float(getNrEvents(datasetCode))

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
  #  datasetCode=100

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

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("Configuration.StandardSequences.Services_cff")



import sys

#CRABHLTNAMEOVERWRITE
hltName="HLTX"
patCandID=""
process.load("SHarper.TrigNtup.trigNtupMaker_cfi")
process.shNtupliser.datasetCode = 1
process.shNtupliser.sampleWeight = 1

process.shNtupliser.addMet = False
process.shNtupliser.addJets = False
process.shNtupliser.addMuons = False
process.shNtupliser.applyMuonId = False
process.shNtupliser.addCaloTowers = True
process.shNtupliser.addCaloHits = True
process.shNtupliser.addIsolTrks = True
process.shNtupliser.addPFCands = True
process.shNtupliser.addPFClusters = True
process.shNtupliser.addHLTDebug = True

process.shNtupliser.minEtToPromoteSC = 20
process.shNtupliser.fillFromGsfEle = True
process.shNtupliser.minNrSCEtPassEvent = cms.double(-1)
process.shNtupliser.outputGeom = cms.bool(False)

process.shNtupliser.hltProcName = cms.string(hltName)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","",hltName)
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","",hltName)
process.shNtupliser.hbheRecHitsTag = cms.InputTag("reducedHcalRecHits","hbhereco")
process.shNtupliser.addTrigSum = cms.bool(True)
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)
#process.shNtupliser.gsfEleTag = cms.InputTag("gedGsfElectronsTrkIsoCorr")
import os


#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.TFileService.fileName= "OUTPUTFILE"
    #process.shNtupliser.outputFilename= "OUTPUTFILE"
    process.shNtupliser.datasetCode = datasetCode
    process.shNtupliser.sampleWeight = SAMPLEWEIGHT
else:
    print "using user specified filename"
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]
    #process.shNtupliser.outputFilename= sys.argv[len(sys.argv)-1]
    process.shNtupliser.datasetCode = datasetCode
    process.shNtupliser.sampleWeight = getWeight(datasetCode)


#import HLTrigger.HLTfilters.hltHighLevel_cfi
#process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
#process.skimHLTFilter.throw=cms.bool(False)
#process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLTSkim")
#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_RemovePileUpDominatedEventsGen_v1",)

#

if datasetCode>=600:
    process.p = cms.Path(process.skimHLTFilter*process.shNtupliser)
else:
    process.p = cms.Path(process.shNtupliser)

#from SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi import swapHEEPToMiniAOD
#swapHEEPToMiniAOD(process.shNtupliser)
#process.source.eventsToProcess = cms.untracked.VEventRange("1:57017-1:57017",)
#from SHarper.HEEPAnalyzer.heepTools import *
#swapCollection(process,"gsfElectrons","gsfElectronsHEEPCorr")
