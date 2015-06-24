isMC=False

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(1000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
#process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
#process.GlobalTag.globaltag = cms.string('GR10_P_V5::All')
from Configuration.AlCa.autoCond import autoCond
if isMC:
    process.GlobalTag.globaltag = cms.string('MCRUN2_74_V9A')
else:
    process.GlobalTag.globaltag = cms.string('GR_P_V56')

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("Configuration.StandardSequences.Services_cff")



import sys

hltName="REDIGI311X"
#do not remove this comment...
#CRABHLTNAMEOVERWRITE
hltName="HLT"
patCandID=""
process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = 1
process.shNtupliser.sampleWeight = 1
process.shNtupliser.gsfEleTag = "gsfElectrons"
process.shNtupliser.addMet = False
process.shNtupliser.addJets = False
process.shNtupliser.addMuons = False
process.shNtupliser.applyMuonId = False
process.shNtupliser.addCaloTowers = True
process.shNtupliser.addCaloHits = True
process.shNtupliser.addIsolTrks = True
process.shNtupliser.addPFCands = True
process.shNtupliser.addPFClusters = True

process.shNtupliser.minEtToPromoteSC = 20
process.shNtupliser.fillFromGsfEle = True
process.shNtupliser.minNrSCEtPassEvent = cms.double(-1)
process.shNtupliser.outputGeom = cms.bool(False)
process.shNtupliser.useHLTDebug = cms.bool(False)
process.shNtupliser.hltProcName = hltName
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","",hltName)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","",hltName)
process.shNtupliser.compTwoMenus = cms.bool(False)
process.shNtupliser.secondHLTTag = cms.string("")
process.shNtupliser.electronTag = cms.untracked.InputTag("patElectrons"+patCandID)
process.shNtupliser.tauTag = cms.untracked.InputTag("patTaus"+patCandID)
process.shNtupliser.muonTag = cms.untracked.InputTag("patMuons"+patCandID)
process.shNtupliser.jetTag = cms.untracked.InputTag("patJets"+patCandID)
process.shNtupliser.photonTag = cms.untracked.InputTag("patPhotons"+patCandID)
process.shNtupliser.metTag = cms.untracked.InputTag("patMETs"+patCandID)
process.shNtupliser.hbheRecHitsTag = cms.InputTag("reducedHcalRecHits","hbhereco")
process.shNtupliser.nrGenPartToStore = cms.int32(-1)
#process.shNtupliser.eleRhoCorrTag = cms.InputTag("kt6PFJets","rho")
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


process.source = cms.Source("PoolSource",
                                #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                                #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                             fileNames = cms.untracked.vstring(),
                             )

isCrabJob=False #script seds this if its a crab job

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.TFileService.fileName= "OUTPUTFILE"
    #process.shNtupliser.outputFilename= "OUTPUTFILE"
    process.shNtupliser.datasetCode = DATASETCODE
    process.shNtupliser.sampleWeight = SAMPLEWEIGHT
else:
    print "using user specified filename"
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]
    #process.shNtupliser.outputFilename= sys.argv[len(sys.argv)-1]
  
    filePrefex="file:"
    if(sys.argv[2].find("/pnfs/")==0):
        filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

    if(sys.argv[2].find("/store/")==0):
        filePrefex=""
    if(sys.argv[2].find("/eos/")==0):
        filePrefex="'root://eoscms/"
  
    for i in range(2,len(sys.argv)-1):
        print filePrefex+sys.argv[i]
        process.source.fileNames.extend([filePrefex+sys.argv[i],])

    from SHarper.SHNtupliser.datasetCodes import getDatasetCode
    process.shNtupliser.datasetCode = getDatasetCode(process.source.fileNames[0])
    process.shNtupliser.sampleWeight = 1
  #  print "datset code ",process.shNtupliser.datasetCode


#process.load('EgammaAnalysis.ElectronTools.egmGsfElectronIDs_cff')

# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_L1SingleEG20_v*")
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
                                    requireEcalDriven=cms.bool(True)
                                     )

print "dataset code: ",process.shNtupliser.datasetCode.value()
if process.shNtupliser.datasetCode.value()>=600 and process.shNtupliser.datasetCode.value()<700:
    print "applying filter for 1 ele and disabling large collections"
    process.egammaFilter.nrElesRequired=cms.int32(1)
    process.shNtupliser.nrGenPartToStore = cms.int32(0)

if process.shNtupliser.datasetCode.value()>=310 and process.shNtupliser.datasetCode.value()<350:
    print "applying filter for 1 ele and disabling large collections"
    process.egammaFilter.nrElesRequired=cms.int32(1)
    process.shNtupliser.nrGenPartToStore = cms.int32(0)


#from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFElectronIso
#process.eleIsoSequence = setupPFElectronIso(process, 'gedGsfElectrons')
process.p = cms.Path(#process.primaryVertexFilter*
    #process.gsfElectronsHEEPCorr*process.eIdSequence*
   process.egammaFilter*
 #   process.pfParticleSelectionSequence* process.eleIsoSequence*
#    process.kt6PFJets*
 #   process.egmGsfElectronIDSequence*
    process.shNtupliser)
        
if not isMC:
    process.p.insert(0,process.skimHLTFilter)

#from SHarper.HEEPAnalyzer.heepTools import *
#swapCollection(process,"gsfElectrons","gsfElectronsHEEPCorr")
