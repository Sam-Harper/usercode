isMC=True

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(1),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = cms.string('GR10_P_V5::All')
from Configuration.AlCa.autoCond import autoCond
if isMC:
    process.GlobalTag.globaltag = autoCond['startup'] 
else:
    process.GlobalTag.globaltag = autoCond['com10']

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(20)
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
process.shNtupliser.addCaloTowers = False
process.shNtupliser.addCaloHits = False
process.shNtupliser.addIsolTrks = False
process.shNtupliser.addPFCands = False

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
process.shNtupliser.nrGenPartToStore = cms.int32(20)
#process.shNtupliser.eleRhoCorrTag = cms.InputTag("kt6PFJets","rho")
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)

import os
cmsswVersion = os.environ['CMSSW_VERSION']
if "CMSSW_7" in cmsswVersion:
    process.shNtupliser.recoPhoTag = "gedPhotons"
    process.shNtupliser.gsfEleTag = "gedGsfElectrons"

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
    process.shNtupliser.datasetCode = 11000
    process.shNtupliser.sampleWeight = 1




filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""
if(sys.argv[2].find("/eos/")==0):
    filePrefex="'root://eoscms/"



process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])

process.load("Configuration.EventContent.EventContent_cff")
process.out = cms.OutputModule("PoolOutputModule",
    process.FEVTEventContent,
    dataset = cms.untracked.PSet(dataTier = cms.untracked.string('RECO')),
     fileName = cms.untracked.string("ihateyoupatdevelopersIreallydo.root"),
)
#process.out.outputCommands = cms.untracked.vstring('drop *','keep *_MEtoEDMConverter_*_RelValTest')
#
process.outPath = cms.EndPath(process.out)

#process.load("CommonTools.ParticleFlow.Isolation.pfElectronIsolation_cff")
from RecoJets.JetProducers.kt4PFJets_cfi import *
process.kt6PFJets = kt4PFJets.clone(
    rParam = cms.double(0.6),
    doAreaFastjet = cms.bool(True),
    doRhoFastjet = cms.bool(True)
) 
process.load('EgammaAnalysis.ElectronTools.egmGsfElectronIDs_cff')


#from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFElectronIso
#process.eleIsoSequence = setupPFElectronIso(process, 'gedGsfElectrons')
process.p = cms.Path(#process.primaryVertexFilter*
    #process.gsfElectronsHEEPCorr*process.eIdSequence*
   # process.egammaFilter*
 #   process.pfParticleSelectionSequence* process.eleIsoSequence*
#    process.kt6PFJets*
    process.egmGsfElectronIDSequence*
    process.shNtupliser)
        

#from SHarper.HEEPAnalyzer.heepTools import *
#swapCollection(process,"gsfElectrons","gsfElectronsHEEPCorr")
