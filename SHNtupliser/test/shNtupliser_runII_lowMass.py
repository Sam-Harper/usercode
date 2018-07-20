isCrabJob=False #script seds this if its a crab job
useMiniAOD=True

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
process = cms.Process("HEEP")

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),  
#                            eventsToProcess = cms.untracked.VEventRange("281707:47701394-281707:47701394")

                            )
if isCrabJob:
    datasetCode=DATASETCODE
else:
    from SHarper.SHNtupliser.addInputFiles import addInputFiles
    addInputFiles(process.source,sys.argv[2:len(sys.argv)-1])
    from SHarper.SHNtupliser.datasetCodes import getDatasetCode
    datasetCode=getDatasetCode(process.source.fileNames[0])
    datasetCode=101
    datasetCode=0

if datasetCode==0: isMC=False
else: isMC=True

datasetVersion="TOSED:DATASETVERSION"
if not isCrabJob:
    try:
        datasetVersion=sys.argv[2].split("/")[-1].split("_")[1]
    except IndexError:
        pass

print "isCrab = ",isCrabJob,"isMC = ",isMC," datasetCode = ",datasetCode," useMiniAOD = ",useMiniAOD,"datasetVersion = ",datasetVersion

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(100000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

#Load geometry
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.autoCond import autoCond
from Configuration.AlCa.GlobalTag import GlobalTag
if isMC:
    process.GlobalTag = GlobalTag(process.GlobalTag, '94X_mc2017_realistic_v10', '')
else:
    from SHarper.SHNtupliser.globalTags_cfi import getGlobalTagNameData
    globalTagName = getGlobalTagNameData(datasetVersion)
    process.GlobalTag = GlobalTag(process.GlobalTag, globalTagName,'')
    process.GlobalTag = GlobalTag(process.GlobalTag, '101X_dataRun2_Prompt_v9', '')

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")
process.load("Configuration.StandardSequences.Services_cff")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

#CRABHLTNAMEOVERWRITE
hltName="HLT"

process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = 1
process.shNtupliser.sampleWeight = 1

process.shNtupliser.addMuons = False
process.shNtupliser.outputGeom = cms.bool(False)
process.shNtupliser.hltProcName = cms.string(hltName)
process.shNtupliser.addEleUserData = cms.bool(True)  
process.shNtupliser.fillTrkIsolFromUserData = cms.bool(True)
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","",hltName)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","",hltName)
process.shNtupliser.hbheRecHitsTag = cms.InputTag("reducedHcalRecHits","hbhereco")

disableLargeCollections=True
if disableLargeCollections:
    print "*******************************************"
    print "*******disabling large collections*********"
    print "*******************************************"
    process.shNtupliser.addPFCands = False
    process.shNtupliser.addPFClusters = False
    process.shNtupliser.addIsolTrks = False
    #process.shNtupliser.addCaloHits = False


if useMiniAOD:
    from SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi import swapHEEPToMiniAOD
    swapHEEPToMiniAOD(process.shNtupliser)
    

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)
#if not isMC:
#    process.shNtupliser.oldGsfEleTag = cms.InputTag("slimmedElectronsBeforeGSFix")
#    process.shNtupliser.metTag = cms.InputTag("slimmedMETsMuEGClean")


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
    process.shNtupliser.sampleWeight = 1
  #  print "datset code ",process.shNtupliser.datasetCode

# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.throw=cms.bool(False)
datasetName="TOSED:DATASETNAME"


if datasetName=="DoubleEG":
    print "setting up HLT skim for DoubleEG"
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_DoublePhoton33*","HLT_DoubleEle33*","HLT_DoubleEle25*","HLT_DoublePhoton70_v*","HLT_DoublePhoton85_v*","HLT_ECALHT800_v*","HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL*","HLT_DiEle27_WPTightCaloOnly*")
elif datasetName=="SingleElectron":
    print "setting up HLT skim for SingleElectron"
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Ele27_WPTight_Gsf_v*","HLT_Ele32_WPTight_Gsf_v*","HLT_Ele35_WPTight_Gsf_v*","HLT_Ele32_WPTight_Gsf_L1DoubleEG_v*")
#    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")
elif datasetName=="SinglePhoton":
    print "setting up HLT skim for SinglePhoton"
    process.skimHLTFilter.HLTPaths =cms.vstring("HLT_Photon22_v*","HLT_Photon25_v*","HLT_Photon30_v*","HLT_Photon33_v*","HLT_Photon50_v*","HLT_Photon75_v*","HLT_Photon90_v*","HLT_Photon120_v*","HLT_Photon150_v*","HLT_Photon175_v*","HLT_Photon200_v*","HLT_Photon250_NoHE_v*","HLT_Photon300_NoHE_v*")
elif datasetName=="JetHT":
    print "setting up HLT skim for JetHT"
    process.skimHLTFilter.HLTPaths =cms.vstring("HLT_CaloJet500_NoJetID_v*","HLT_CaloJet550_NoJetID_v*",)
else:
    print "setting HLT skim to select all"
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")

process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")

process.egammaFilter = cms.EDFilter("EGammaLowMassTPFilter",
                                    eleTag=process.shNtupliser.gsfEleTag,
                                    tagId=cms.string("cutBasedElectronID-Fall17-94X-V1-loose"),
                                    minMass=cms.double(1),
                                    maxMass=cms.double(20),
                                    )

print "dataset code: ",process.shNtupliser.datasetCode.value()

if process.shNtupliser.datasetCode.value()>=140 and process.shNtupliser.datasetCode.value()<1000:
    print "applying filter for 1 ele and disabling large collections"
    process.egammaFilter.nrElesRequired=cms.int32(1)
    process.shNtupliser.nrGenPartToStore = cms.int32(0)
    process.shNtupliser.addPFCands = False
    process.shNtupliser.addPFClusters = False
    process.shNtupliser.addIsolTrks = False

if isCrabJob and process.shNtupliser.datasetCode.value()>140:
    process.shNtupliser.addTrigSum = cms.bool(False)

process.p = cms.Path(
    process.egammaFilter*
    process.shNtupliser)
        
if not isMC:
    process.p.insert(0,process.skimHLTFilter)


#process.AODSIMoutput = cms.OutputModule("PoolOutputModule",
#    compressionAlgorithm = cms.untracked.string('LZMA'),
#    compressionLevel = cms.untracked.int32(4),
#    dataset = cms.untracked.PSet(
#        dataTier = cms.untracked.string('AODSIM'),
#        filterName = cms.untracked.string('')
#    ),
#    eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
#    fileName = cms.untracked.string('file:outputTestAOD.root'),
#    outputCommands = cms.untracked.vstring("keep *_*_*_*",)
#)                                        
#process.out = cms.EndPath(process.AODSIMoutput)
print process.GlobalTag.globaltag
