isCrabJob=False #script seds this if its a crab job
useMiniAOD=True

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
process = cms.Process("HEEP")

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.parseArguments()

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles),  
#                            eventsToProcess = cms.untracked.VEventRange("281707:47701394-281707:47701394")

                          )

if isCrabJob:
    datasetCode=DATASETCODE
else:
    #from SHarper.SHNtupliser.addInputFiles import addInputFiles
   # addInputFiles(process.source,options.inputFiles)
    #from SHarper.SHNtupliser.datasetCodes import getDatasetCode
    #datasetCode=getDatasetCode(process.source.file])
#    datasetCode=101
    datasetCode=0

if datasetCode==0: isMC=False
else: isMC=True

datasetVersion="TOSED:DATASETVERSION"


print "isCrab = ",isCrabJob,"isMC = ",isMC," datasetCode = ",datasetCode," useMiniAOD = ",useMiniAOD,"datasetVersion = ",datasetVersion

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

#useMiniAOD=False

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format  to be
# DataFormat.AOD or DataFormat.MiniAOD, as appropriate
if useMiniAOD:
    switchOnVIDElectronIdProducer(process,DataFormat.MiniAOD)
   # switchOnVIDPhotonIdProducer(process,DataFormat.MiniAOD)
else:
    switchOnVIDElectronIdProducer(process,DataFormat.AOD)
    switchOnVIDPhotonIdProducer(process,DataFormat.AOD)


# define which IDs we want to produce and add them to VID
ele_id_modules =  [ 'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff',
                    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V1_cff',
                    'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_noIso_V1_cff', 
                    'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_iso_V1_cff',
                    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
                    'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_GeneralPurpose_V1_cff',
                    'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_HZZ_V1_cff',
                  ]
pho_id_modules =  [ 'RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Fall17_94X_V1_TrueVtx_cff',
                    'RecoEgamma.PhotonIdentification.Identification.mvaPhotonID_Fall17_94X_V1_cff', 
                    'RecoEgamma.PhotonIdentification.Identification.mvaPhotonID_Fall17_94X_V1p1_cff', 
                    'RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring16_V2p2_cff',
                    'RecoEgamma.PhotonIdentification.Identification.mvaPhotonID_Spring16_nonTrig_V1_cff'
                  ]
for idmod in ele_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)
#for idmod in pho_id_modules:
#    setupAllVIDIdsInModule(process,idmod,setupVIDPhotonSelection)


if useMiniAOD:
    process.egmPhotonIDs = cms.PSet(physicsObjectIDs = cms.VPSet())
    process.load('EgammaAnalysis.ElectronTools.calibratedPatElectronsRun2_cfi')
    process.load('EgammaAnalysis.ElectronTools.calibratedPatPhotonsRun2_cfi')
    process.calibratedPatElectrons.electrons = cms.InputTag('slimmedElectrons',processName=cms.InputTag.skipCurrentProcess())
    process.calibratedPatPhotons.photons = cms.InputTag('slimmedPhotons',processName=cms.InputTag.skipCurrentProcess())
    process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('calibratedPatElectrons')
 #   process.egmPhotonIDs.physicsObjectSrc = cms.InputTag('calibratedPatPhotons')
    process.electronMVAValueMapProducer.srcMiniAOD = cms.InputTag('calibratedPatElectrons') 
 #   process.photonMVAValueMapProducer.srcMiniAOD = cms.InputTag('calibratedPatPhotons')
#    process.photonIDValueMapProducer.srcMiniAOD = cms.InputTag('calibratedPatPhotons')
  #  process.egmPhotonIsolation.srcToIsolate = cms.InputTag('calibratedPatPhotons')
    if hasattr(process,'heepIDVarValueMaps'):
        process.heepIDVarValueMaps.elesMiniAOD = cms.InputTag('calibratedPatElectrons')

    from RecoEgamma.EgammaTools.egammaObjectModificationsInMiniAOD_cff import egamma_modifications
    from RecoEgamma.EgammaTools.egammaObjectModifications_tools import makeVIDBitsModifier,makeVIDinPATIDsModifier,makeEnergyScaleAndSmearingSysModifier                                     
    egamma_modifications.append(makeVIDBitsModifier(process,"egmGsfElectronIDs","egmPhotonIDs"))
    egamma_modifications.append(makeVIDinPATIDsModifier(process,"egmGsfElectronIDs","egmPhotonIDs"))
    egamma_modifications.append(makeEnergyScaleAndSmearingSysModifier("calibratedPatElectrons","calibratedPatPhotons"))

    #add the HEEP trk isol to the slimmed electron
    egamma_modifications[0].electron_config.heepV70TrkPtIso = cms.InputTag("heepIDVarValueMaps","eleTrkPtIso")
    for pset in egamma_modifications:
        pset.overrideExistingValues = cms.bool(True)
        if hasattr(pset,"electron_config"): pset.electron_config.electronSrc = cms.InputTag("calibratedPatElectrons")
        if hasattr(pset,"photon_config"): pset.photon_config.photonSrc = cms.InputTag("calibratedPatPhotons")

    process.slimmedElectrons = cms.EDProducer("ModifiedElectronProducer",
                                              src=cms.InputTag("calibratedPatElectrons"),
                                              modifierConfig = cms.PSet(
            modifications = egamma_modifications
            )
                                              )
    process.slimmedPhotons = cms.EDProducer("ModifiedPhotonProducer",
                                              src=cms.InputTag("calibratedPatPhotons"),
                                              modifierConfig = cms.PSet(
            modifications = cms.VPSet()# egamma_modifications
            )
                                            )

                                            
    


else:
    process.load('EgammaAnalysis.ElectronTools.calibratedElectronsRun2_cfi')
    process.load('EgammaAnalysis.ElectronTools.calibratedPhotonsRun2_cfi')
    #we will rename the collections to the standard names so VID and other things pick it up

    process.gedGsfElectrons = process.calibratedElectrons.clone(electrons=cms.InputTag("gedGsfElectrons",processName=cms.InputTag.skipCurrentProcess()))
    process.gedPhotons = process.calibratedPhotons.clone(photons=cms.InputTag("gedPhotons",processName=cms.InputTag.skipCurrentProcess())) 

if useMiniAOD:
    process.egammaScaleSmearTask = cms.Task(process.calibratedPatElectrons,process.slimmedElectrons,
                                            process.calibratedPatPhotons,process.slimmedPhotons
                                            )
else:
    process.egammaScaleSmearTask = cms.Task(process.gedGsfElectrons,
                                            process.gedPhotons
                                            )
                                          

process.egammaScaleSmearSeq = cms.Sequence( process.egammaScaleSmearTask)
process.egammaScaleSmearAndVIDSeq = cms.Sequence(process.egammaScaleSmearSeq*
    process.egmGsfElectronIDSequence)
    #process.egmPhotonIDSequence)


process.p = cms.Path(#process.primaryVertexFilter*
  #  process.regressionApplication*
    process.egammaScaleSmearAndVIDSeq*
 #   process.eleTrkIsol*
    process.shNtupliser)
 

if not isMC and False:
    print "overriding"
    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.l1Menu = cms.ESSource("PoolDBESSource",CondDBSetup,
                                  connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                                  toGet = cms.VPSet(cms.PSet(record = cms.string("L1TGlobalPrescalesVetosRcd"),
                                                             tag = cms.string("L1TGlobalPrescalesVetos_Stage2v0_hlt")),
                                                    cms.PSet(record = cms.string("L1TUtmTriggerMenuRcd"),
                                                             tag = cms.string("L1TUtmTriggerMenu_Stage2v0_hlt"))
                                                    )                              )
    process.es_prefer_l1Menu = cms.ESPrefer("PoolDBESSource","l1Menu")

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
    fileName = cms.untracked.string('file:outputTestAOD.root'),
    outputCommands = cms.untracked.vstring('keep *',
                                           "keep *_*_*_RECO",
                                           'keep *_*_*_HLT',
                                           'keep *_slimmedElectrons*_*_*',
                                           'keep *_slimmedPhotons*_*_*')
                                           
)                                        
#process.out = cms.EndPath(process.AODSIMoutput)
print process.GlobalTag.globaltag
