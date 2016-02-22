isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
                            inputCommands=cms.untracked.vstring('keep *',
                                                                'drop *_hltEgammaPixelSeedVars*_*_*')
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
    datasetCode=101

if datasetCode==0: isMC=False
else: isMC=True

print "isCrab = ",isCrabJob,"isMC = ",isMC," datasetCode = ",datasetCode

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(5000),
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
hltName="HLT"
patCandID=""
process.load("SHarper.SHNtupliser.shNtupliser_cfi")
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
process.shNtupliser.addHLTDebug = cms.bool(True)
process.shNtupliser.minEtToPromoteSC = 20
process.shNtupliser.fillFromGsfEle = True
process.shNtupliser.minNrSCEtPassEvent = cms.double(-1)
process.shNtupliser.outputGeom = cms.bool(False)

process.shNtupliser.hltProcName = cms.string("HLTX")
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLTX")
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","","HLTX")
process.shNtupliser.gsfEleTag = "gsfElectrons"
process.shNtupliser.electronTag = cms.untracked.InputTag("patElectrons"+patCandID)
process.shNtupliser.tauTag = cms.untracked.InputTag("patTaus"+patCandID)
process.shNtupliser.muonTag = cms.untracked.InputTag("patMuons"+patCandID)
process.shNtupliser.jetTag = cms.untracked.InputTag("patJets"+patCandID)
process.shNtupliser.photonTag = cms.untracked.InputTag("patPhotons"+patCandID)
process.shNtupliser.metTag = cms.untracked.InputTag("patMETs"+patCandID)
process.shNtupliser.hbheRecHitsTag = cms.InputTag("reducedHcalRecHits","hbhereco")
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)

import os
cmsswVersion = os.environ['CMSSW_VERSION']
if "CMSSW_7" in cmsswVersion:
    process.shNtupliser.recoPhoTag = "gedPhotons"
    process.shNtupliser.gsfEleTag = "gedGsfElectrons"
    process.shNtupliser.superClusterEBTag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALBarrel")
    process.shNtupliser.superClusterEETag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALEndcapWithPreshower")


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
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")
#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")

process.egammaFilter = cms.EDFilter("EGammaFilter",
                                    nrElesRequired=cms.int32(-1),
                                    nrPhosRequired=cms.int32(-1),
                                    nrSCsRequired=cms.int32(-1),
                                    eleEtCut=cms.double(20),
                                    phoEtCut=cms.double(20),
                                    scEtCut=cms.double(-1),
                                    eleTag=process.shNtupliser.gsfEleTag,
                                    phoTag=process.shNtupliser.recoPhoTag,
                                    superClusEBTag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALBarrel"),
                                    superClusEETag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALEndcapWithPreshower"),
                                    caloTowerTag = cms.InputTag("towerMaker"),
                                    genEvtInfoTag=cms.InputTag("generator"),
                                    requireEcalDriven=cms.bool(True)
                                     )

print "dataset code: ",process.shNtupliser.datasetCode.value()
if process.shNtupliser.datasetCode.value()>=800 and process.shNtupliser.datasetCode.value()<700:
    print "applying filter for 1 ele and disabling large collections"
    process.egammaFilter.nrElesRequired=cms.int32(1)
    process.shNtupliser.nrGenPartToStore = cms.int32(0)

if process.shNtupliser.datasetCode.value()>140 and process.shNtupliser.datasetCode.value()<1000:
    print "applying filter for 1 ele and disabling large collections"
    process.egammaFilter.nrElesRequired=cms.int32(1)
    process.shNtupliser.nrGenPartToStore = cms.int32(0)


process.hltEgammaPixelSeedVars = cms.EDProducer("EgammaHLTPixelMatchVarProducer",
                                                recoEcalCandidateProducer=cms.InputTag("hltEgammaCandidates"),
                                                pixelSeedsProducer=cms.InputTag("hltEgammaElectronPixelSeeds")
)
process.hltEgammaPixelSeedVarsUnseeded = cms.EDProducer("EgammaHLTPixelMatchVarProducer",
                                                        recoEcalCandidateProducer=cms.InputTag("hltEgammaCandidatesUnseeded"),
                                                        pixelSeedsProducer=cms.InputTag("hltEgammaElectronPixelSeedsUnseeded")
)
process.p = cms.Path(#process.primaryVertexFilter*
#    process.egammaFilter*
    process.hltEgammaPixelSeedVars*process.hltEgammaPixelSeedVarsUnseeded*
    process.shNtupliser)
        
if not isMC:
    process.p.insert(0,process.skimHLTFilter)

#from SHarper.HEEPAnalyzer.heepTools import *
#swapCollection(process,"gsfElectrons","gsfElectronsHEEPCorr")
