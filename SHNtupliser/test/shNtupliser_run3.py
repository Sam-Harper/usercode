isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
from Configuration.Eras.Era_Run3_cff import Run3
process = cms.Process("HEEP",Run3)

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.register('isMiniAOD',True,options.multiplicity.singleton,options.varType.bool," whether we are running on miniAOD or not")
options.register('datasetCode',0,options.multiplicity.singleton,options.varType.int," datasetcode")
options.register('datasetName',"",options.multiplicity.singleton,options.varType.string," datasetName")


options.parseArguments()

print(options.inputFiles)
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles),  
                          )

if isCrabJob:
    datasetCode=DATASETCODE
else:
    datasetCode=options.datasetCode

if datasetCode==0: isMC=False
else: isMC=True

datasetVersion="TOSED:DATASETVERSION"


print("isCrab = ",isCrabJob,"isMC = ",isMC," datasetCode = ",datasetCode," useMiniAOD = ",options.isMiniAOD,"datasetVersion = ",datasetVersion)

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(5000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

#Load geometry
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.autoCond import autoCond
from Configuration.AlCa.GlobalTag import GlobalTag
if isMC:
    process.GlobalTag = GlobalTag(process.GlobalTag, '102X_upgrade2018_realistic_v15', '')
else:
    from SHarper.SHNtupliser.globalTags_cfi import getGlobalTagNameData
    globalTagName = getGlobalTagNameData(datasetVersion)
    process.GlobalTag = GlobalTag(process.GlobalTag, globalTagName,'')
    process.GlobalTag = GlobalTag(process.GlobalTag, '130X_dataRun3_PromptAnalysis_v1','')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")
process.load("Configuration.StandardSequences.Services_cff")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

#CRABHLTNAMEOVERWRITE
hltName="HLT"

process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = 1
process.shNtupliser.sampleWeight = 1

process.shNtupliser.addMuons = False
process.shNtupliser.outputGeom = cms.bool(False)
process.shNtupliser.hltProcName = cms.string(hltName)
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","",hltName)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","",hltName)
process.shNtupliser.hbheRecHitsTag = cms.InputTag("reducedHcalRecHits","hbhereco")
process.shNtupliser.addEleUserData = cms.bool(True)  
process.shNtupliser.fillTrkIsolFromUserData = cms.bool(False)
process.shNtupliser.heepIDVID = cms.InputTag("")
process.shNtupliser.heepIDVIDBits = cms.InputTag("")
process.shNtupliser.vidBits = cms.VInputTag()
process.shNtupliser.eleIsolPtTrksValueMapTag = cms.InputTag("")
process.shNtupliser.trkIsoNoJetCoreTag = cms.InputTag("")
process.shNtupliser.nrSatCrysIn5x5Tag = cms.InputTag("") 
process.shNtupliser.addPFCands = True
process.shNtupliser.stageL1Trigger = cms.uint32(2)
process.shNtupliser.minEtToPromoteSC = 20
process.shNtupliser.minEtToSaveEle = 20
disableLargeCollections=True
if disableLargeCollections:
    print("*******************************************")
    print("*******disabling large collections*********")
    print("*******************************************")
    process.shNtupliser.addPFCands = False
    process.shNtupliser.addPFClusters = True
    process.shNtupliser.addIsolTrks = True
    process.shNtupliser.addCaloHits = True


if options.isMiniAOD:
    from SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi import swapHEEPToMiniAOD
    swapHEEPToMiniAOD(process.shNtupliser)

    #process.shNtupliser.oldPhoTag = cms.InputTag("slimmedPhotonsReg")
    #process.shNtupliser.oldGsfEleTag = cms.InputTag("slimmedElectronsReg")
    #process.load("SHarper.SHNtupliser.regressionApplicationMiniAOD_newNames_cff")

process.load("SHarper.SHNtupliser.egammaFilter_cfi")

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)


#if 1, its a crab job...
if isCrabJob:
    print("using crab specified filename")
    process.TFileService.fileName= "OUTPUTFILE"
    #process.shNtupliser.outputFilename= "OUTPUTFILE"
    process.shNtupliser.datasetCode = datasetCode
    process.shNtupliser.sampleWeight = SAMPLEWEIGHT
else:
    print("using user specified filename")
    process.TFileService.fileName= options.outputFile
    #process.shNtupliser.outputFilename= sys.argv[len(sys.argv)-1]
    process.shNtupliser.datasetCode = datasetCode
    process.shNtupliser.sampleWeight = 1
  #  print "datset code ",process.shNtupliser.datasetCode

print("dataset code: ",process.shNtupliser.datasetCode.value())

if process.shNtupliser.datasetCode.value()>=140 and process.shNtupliser.datasetCode.value()<1000:
    print("applying filter for 1 ele and disabling large collections")
    process.egammaFilter.nrElesRequired=cms.int32(1)
    process.shNtupliser.nrGenPartToStore = cms.int32(0)
    process.shNtupliser.addPFCands = False
    process.shNtupliser.addPFClusters = False
    process.shNtupliser.addIsolTrks = False

# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.throw=cms.bool(False)

if isCrabJob:
    datasetName="TOSED:DATASETNAME"
else:
    datasetName = options.datasetName

datasetName = datasetName.rstrip("0123456789")
if datasetName=="DoubleEG":
    print("setting up HLT skim for DoubleEG")
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_DoubleEle33*","HLT_DoubleEle25*","HLT_DoubleEle37*","HLT_DoublePhoton60_v*","HLT_DoublePhoton70_v*","HLT_DoublePhoton85_v*","HLT_ECALHT800_v*","HLT_Ele23_Ele12_CaloIdL_TrackIdL*","HLT_DiEle27_WPTightCaloOnly_*")
elif datasetName=="SingleElectron":
    print("setting up HLT skim for SingleElectron")
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Ele105_CaloIdVT_GsfTrkIdT_v*","HLT_Ele115_CaloIdVT_GsfTrkIdT_v*","HLT_Ele27_WPTight_Gsf_v*","HLT_Ele32_WPTight_Gsf_*")
elif datasetName=="SinglePhoton":
    print("setting up HLT skim for SinglePhoton")
    process.skimHLTFilter.HLTPaths =cms.vstring("HLT_Photon22_v*","HLT_Photon30_v*","HLT_Photon33_v*","HLT_Photon36_v*","HLT_Photon50_v*","HLT_Photon75_v*","HLT_Photon90_v*","HLT_Photon120_v*","HLT_Photon165_HE10_v*","HLT_Photon175_v*","HLT_Photon200_v*","HLT_Photon250_NoHE_v*","HLT_Photon300_NoHE_v*")
elif datasetName=="EGamma":
    print("setting up HLT skim for EGamma")
    process.skimHLTFilter.HLTPaths =cms.vstring("HLT_DoubleEle33*","HLT_DoubleEle25*","HLT_DoubleEle27*","HLT_DoublePhoton33*","HLT_DoublePhoton70_v*","HLT_DoublePhoton85_v*","HLT_ECALHT800_v*","HLT_Ele23_Ele12_CaloIdL_TrackIdL*","HLT_DiEle27_WPTightCaloOnly_*","HLT_Ele115_CaloIdVT_GsfTrkIdT_v*","HLT_Ele27_WPTight_Gsf_v*","HLT_Ele30_WPTight_Gsf_*", "HLT_Ele32_WPTight_Gsf_*","HLT_Photon33_v*","HLT_Photon50_v*","HLT_Photon75_v*","HLT_Photon90_v*","HLT_Photon120_v*","HLT_Photon175_v*","HLT_Photon200_v*","HLT_Photon300_NoHE_v*")
elif datasetName=="JetHT":
    print("setting up HLT skim for JetHT")
    process.skimHLTFilter.HLTPaths =cms.vstring("HLT_CaloJet500_NoJetID_v*",)
else:
    print("setting HLT skim to select all")
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")

if isCrabJob and process.shNtupliser.datasetCode.value()>140:
    process.shNtupliser.addTrigSum = cms.bool(False)

#from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
#setupEgammaPostRecoSeq(process,
#                       runEnergyCorrections=True, 
#                       runVID=False,
#                       era='2018-Prompt')  
#                       era='2017-Nov17ReReco')  
#a sequence egammaPostRecoSeq has now been created and should be added to your path, eg process.p=cms.Path(process.egammaPostRecoSeq)

process.p = cms.Path(
   #c process.egammaPostRecoSeq*
 #   process.regressionApplication*
    process.shNtupliser)

filterEles=True
if filterEles:
    process.egammaFilter.nrElesRequired = 2
    process.egammaFilter.eleTag = "slimmedElectrons"
    process.p.insert(0,process.egammaFilter)

if not isMC:
    process.p.insert(0,process.skimHLTFilter)

    
process.AODSIMoutput = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(4),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('AODSIM'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
    fileName = cms.untracked.string(options.outputFile.replace(".root","_EDM.root")),
    outputCommands = cms.untracked.vstring('keep *',
                                           "keep *_*_*_RECO",
                                           'keep *_*_*_HLT',
                                           'keep *_slimmedElectrons*_*_*',
                                           'keep *_slimmedPhotons*_*_*')
                                           
)      

if not isCrabJob:                                  
    #process.out = cms.EndPath(process.AODSIMoutput)
    pass 
print(process.GlobalTag.globaltag)





def setEventsToProcess(process,eventsToProcess):
    process.source.eventsToProcess = cms.untracked.VEventRange()
    for event in eventsToProcess:
        runnr = event.split(":")[0]
        eventnr = event.split(":")[2]
        process.source.eventsToProcess.append('{runnr}:{eventnr}-{runnr}:{eventnr}'.format(runnr=runnr,eventnr=eventnr))

#eventsToProcess = ['1:8706:8705770']
#setEventsToProcess(process,eventsToProcess)
