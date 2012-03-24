#shamelessly stolen from PatAnalyzerSkeleton_cfg.py

isMC=False

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkSummary = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

process.GlobalTag.globaltag = cms.string('GR_R_42_V21::All')

process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")


import sys
process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = 0
process.shNtupliser.sampleWeight = 1
process.shNtupliser.gsfEleTag = "gsfElectrons"
process.shNtupliser.addMet = True
process.shNtupliser.addJets = True
process.shNtupliser.addMuons = True

process.shNtupliser.minEtToPromoteSC = 20;
process.shNtupliser.fillFromGsfEle = True
process.shNtupliser.minSCEtToPass = cms.double(-1)
process.shNtupliser.electronTag = cms.untracked.InputTag("patElectronsPFlow")
process.shNtupliser.tauTag = cms.untracked.InputTag("patTausPFlow")
process.shNtupliser.muonTag = cms.untracked.InputTag("patMuonsPFlow")
process.shNtupliser.jetTag = cms.untracked.InputTag("selectedPatJetsPFlow")
process.shNtupliser.photonTag = cms.untracked.InputTag("patPhotonsPFlow")
process.shNtupliser.metTag = cms.untracked.InputTag("patMETsPFlow")

hltProcName = "HLT"

process.shNtupliser.hltProcName = hltProcName
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","",hltProcName)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","",hltProcName)

# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()

phoSkim=True
#PHOSKIMOVERWRITE
if phoSkim:
    #process.skimHLTFilter.HLTPaths = cms.vstring("HLT_DoublePhoton33*","HLT_Photon*","HLT_DoubleEle33*")
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_DoublePhoton33_v*","HLT_DoublePhoton50_v*","HLT_DoublePhoton60_v*","HLT_DoublePhoton70_v*","HLT_DoublePhoton80_v*", #all the double photons
                                                 "HLT_Photon26_Photon18_v*","HLT_Photon36_Photon22_v*", #prescale double photon
                                                 "HLT_Photon*CaloIdVL_v*","HLT_Photon125_v*","HLT_Photon135_v*","HLT_Photon*NoHE*", #single photon
                                                 "HLT_DoubleEle33_CaloId*","HLT_DoubleEle45_CaloIdL_v*")
else:
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Ele17_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_Ele8_Mass30_v*","HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v*")


process.skimHLTFilter.throw = False


isCrabJob=False

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.shNtupliser.outputFilename= "OUTPUTFILE"
    process.shNtupliser.datasetCode = DATASETCODE
    #process.shNtupliser.sampleWeight = SAMPLEWEIGHT
    process.shNtupliser.sampleWeight = 1
else:
    print "using user specified filename"
    process.shNtupliser.outputFilename= sys.argv[len(sys.argv)-1]
    process.shNtupliser.datasetCode = 0
    process.shNtupliser.sampleWeight = 1


process.shNtupliser.hltDebugFiltersToSave = cms.vstring()

filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

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
process.out.outputCommands = cms.untracked.vstring('drop *','keep *_MEtoEDMConverter_*_RelValTest')


#process.myPref = cms.ESPrefer("PoolDBESSource","GlobalTag")

process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                                      vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                                      minimumNDOF = cms.uint32(4) ,
 						      maxAbsZ = cms.double(24),	
 						      maxd0 = cms.double(2)	
                                                      )






runningOn35XSample=False
#the below comment is really really important, my scripts reset this
#CRAB35XSAMPLEOVERWRITE


process.load("PhysicsTools.PatAlgos.patSequences_cff")


# Get a list of good primary vertices, in 42x, these are DAF vertices
from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
process.goodOfflinePrimaryVertices = cms.EDFilter(
    "PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
    src=cms.InputTag('offlinePrimaryVertices')
    )


# Configure PAT to use PF2PAT instead of AOD sources
# this function will modify the PAT sequences. It is currently 
# not possible to run PF2PAT+PAT and standart PAT at the same time
from PhysicsTools.PatAlgos.tools.pfTools import *
postfix = "PFlow"
usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC=isMC, postfix=postfix)
process.pfPileUpPFlow.Enable = True
process.pfPileUpPFlow.checkClosestZVertex = cms.bool(False)
process.pfPileUpPFlow.Vertices = cms.InputTag('goodOfflinePrimaryVertices')
process.pfJetsPFlow.doAreaFastjet = True
process.pfJetsPFlow.doRhoFastjet = False


# Compute the mean pt per unit area (rho) from the
# PFchs inputs
from RecoJets.JetProducers.kt4PFJets_cfi import kt4PFJets
process.kt6PFJetsPFlow = kt4PFJets.clone(
    rParam = cms.double(0.6),
    src = cms.InputTag('pfNoElectron'+postfix),
    doAreaFastjet = cms.bool(True),
    doRhoFastjet = cms.bool(True)
    )
process.patJetCorrFactorsPFlow.rho = cms.InputTag("kt6PFJetsPFlow", "rho")

# Add the PV selector and KT6 producer to the sequence
getattr(process,"patPF2PATSequence"+postfix).replace(
    getattr(process,"pfNoElectron"+postfix),
    getattr(process,"pfNoElectron"+postfix)*process.kt6PFJetsPFlow )

process.patseq = cms.Sequence(    
    process.goodOfflinePrimaryVertices*
    getattr(process,"patPF2PATSequence"+postfix)
    )

if isMC==False:
    process.patJetCorrFactors.levels = cms.vstring('L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual')
else:
    process.patJetCorrFactors.levels = cms.vstring('L1Offset', 'L2Relative', 'L3Absolute')

process.load('RecoJets.JetProducers.kt4PFJets_cfi')
process.kt6PFJets = process.kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True )
process.kt6PFJets.Rho_EtaMax = cms.double(2.5)

#RunOnData()#,['All'])
#process.patJetCorrFactors.corrSample = cms.string("Summer09_7TeV_ReReco332")
#process.patJetCorrFactors.corrSample = cms.string("Spring10")
# define path 'p': PAT Layer 0, PAT Layer 1, and the analyzer
process.p = cms.Path(process.primaryVertexFilter*
  
                     process.skimHLTFilter*
                     process.patseq*process.kt6PFJets*
                  #   process.patDefaultSequence*
                     process.shNtupliser)

