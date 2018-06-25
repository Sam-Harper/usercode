isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
process = cms.Process("HEEP")

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.register('isMiniAOD',True,options.multiplicity.singleton,options.varType.bool," whether we are running on miniAOD or not")
options.register('applyECorr',False,options.multiplicity.singleton,options.varType.bool," ")
options.register('applyVIDOnECorrEgamma',False,options.multiplicity.singleton,options.varType.bool," ")


options.parseArguments()

print options.inputFiles
if options.inputFiles==[]:
    if options.isMiniAOD:
        options.inputFiles=['file:/opt/ppd/month/harper/dataFiles/DoubleEG_Run2017F_MINIAOD_94X_305064_3E907453-F3E1-E711-BFB4-0CC47AB0B704.root']
    else:
        options.inputFiles=['file:/opt/ppd/month/harper/dataFiles/DoubleEG_Run2017E_AOD_304144_LS90to91_LS629to631_LS961to963_A479BAD3-69D3-E711-AE6D-FA163E41AF41.root']

print options.inputFiles
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles),  
                         #   eventsToProcess = cms.untracked.VEventRange("306138:870375103-306138:870375103")

                          )

if isCrabJob:
    datasetCode=DATASETCODE
else:
    #from SHarper.SHNtupliser.addInputFiles import addInputFiles
   # addInputFiles(process.source,options.inputFiles)
    #from SHarper.SHNtupliser.datasetCodes import getDatasetCode
    #datasetCode=getDatasetCode(process.source.file])
    datasetCode=101
#    datasetCode=0

if datasetCode==0: isMC=False
else: isMC=True

datasetVersion="TOSED:DATASETVERSION"


print "isCrab = ",isCrabJob,"isMC = ",isMC," datasetCode = ",datasetCode," useMiniAOD = ",options.isMiniAOD,"datasetVersion = ",datasetVersion

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(100),
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
    process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_ReReco_EOY17_v2', '')

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")
process.load("Configuration.StandardSequences.Services_cff")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
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
process.shNtupliser.fillTrkIsolFromUserData = cms.bool(True)
process.shNtupliser.heepIDVID = cms.InputTag("")
process.shNtupliser.heepIDVIDBits = cms.InputTag("")
process.shNtupliser.vidBits = cms.VInputTag()
process.shNtupliser.eleIsolPtTrksValueMapTag = cms.InputTag("")
process.shNtupliser.trkIsoNoJetCoreTag = cms.InputTag("")
process.shNtupliser.nrSatCrysIn5x5Tag = cms.InputTag("")
disableLargeCollections=True
if disableLargeCollections:
    print "*******************************************"
    print "*******disabling large collections*********"
    print "*******************************************"
    process.shNtupliser.addPFCands = False
    process.shNtupliser.addPFClusters = False
    process.shNtupliser.addIsolTrks = False
    #process.shNtupliser.addCaloHits = False


if options.isMiniAOD:
    from SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi import swapHEEPToMiniAOD
    swapHEEPToMiniAOD(process.shNtupliser)

    process.shNtupliser.oldPhoTag = cms.InputTag("slimmedPhotonsReg")
    process.shNtupliser.oldGsfEleTag = cms.InputTag("slimmedElectronsReg")


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
    process.TFileService.fileName= options.outputFile
    #process.shNtupliser.outputFilename= sys.argv[len(sys.argv)-1]
    process.shNtupliser.datasetCode = datasetCode
    process.shNtupliser.sampleWeight = 1
  #  print "datset code ",process.shNtupliser.datasetCode

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


from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
setupEgammaPostRecoSeq(process,applyEnergyCorrections=options.applyECorr,
                       applyVIDOnCorrectedEgamma=options.applyVIDOnECorrEgamma,
                       isMiniAOD=options.isMiniAOD,
                       era='2017-Nov17ReReco')
#                       era='2016-Legacy')



process.load("SHarper.SHNtupliser.regressionApplicationMiniAOD_newNames_cff")


# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.throw=cms.bool(False)
datasetName="TOSED:DATASETNAME"


print "setting up HLT skim for DoubleEG"
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL*")
#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")
process.p = cms.Path(#process.primaryVertexFilter*
    process.regressionApplication*
    process.egammaPostRecoSeq*
 #   process.eleTrkIsol*
    process.shNtupliser)
 
if not isMC:
    process.p.insert(0,process.skimHLTFilter)


if not isCrabJob:
    import FWCore.PythonUtilities.LumiList as LumiList
#    process.source.lumisToProcess = LumiList.LumiList(filename = 'crab_projects/crab_Data_DoubleEG_8026_SHv29D_276831-277420_MINIAOD_03Feb2017-v1_20170210_133745_lumis_job69.json').getVLuminosityBlockRange()
#    process.source.lumisToProcess = LumiList.LumiList(filename = 'crab_projects/crab_Data_DoubleEG_8026_SHv29D_281207-284035_MINIAOD_03Feb2017_ver2-v1_20170212_180554_lumis_job172.json').getVLuminosityBlockRange()

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
#process.out = cms.EndPath(process.AODSIMoutput)
print process.GlobalTag.globaltag
if options.isMiniAOD:
    print "pat eles",process.calibratedPatElectrons.correctionFile
else:
    print "gsf eles",process.calibratedElectrons.correctionFile
