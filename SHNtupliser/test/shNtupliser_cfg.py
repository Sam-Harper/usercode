#shamelessly stolen from PatAnalyzerSkeleton_cfg.py

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

process.GlobalTag.globaltag = cms.string('GR_R_311_V2::All')

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

hltProcName = "HLT"

process.shNtupliser.hltProcName = hltProcName
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","",hltProcName)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","",hltProcName)

# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()

phoSkim=False
#PHOSKIMOVERWRITE
if phoSkim:
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_DoublePhoton33*","HLT_Photon*","HLT_DoubleEle33*")
else:
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Ele20_SW_L1R","HLT_Ele17_SW_TightEleId_L1R","HLT_Ele17_SW_TighterEleId_L1R_v1","HLT_Ele_SW_TighterEleId_L1R_v2","HLT_Ele_SW_TighterEleId_L1R_v3");



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
     fileName = cms.untracked.string("eh.root"),
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


#no configuration is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");

# Jet energy corrections to use:
#inputJetCorrLabel = ('AK5PF', ['L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual'])

# Jet energy corrections to use:
#inputJetCorrLabel = ('L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual')
#patJetCorrFactor.levels =cms.vstring('L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual')
# add pf met
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.metTools import *

#addPfMET(process, 'PF')

### Add PF jets
##from PhysicsTools.PatAlgos.tools.jetTools import *
##switchJetCollection(process,cms.InputTag('ak5PFJets'),
##                 doJTA        = True,
##                 doBTagging   = True,
##                 jetCorrLabel = inputJetCorrLabel,
##                 doType1MET   = True,
##                 genJetCollection=cms.InputTag("ak5GenJets"),
##                 doJetID      = True
##                 )
##process.patJets.addTagInfos = True
##process.patJets.tagInfoSources  = cms.VInputTag(
##    cms.InputTag("secondaryVertexTagInfosAOD"),
##    )

process.patJetCorrFactors.levels = cms.vstring('L1Offset', 'L2Relative', 'L3Absolute','L2L3Residual')
removeMCMatching(process, ['All'])
#from PhysicsTools.PatAlgos.patTemplate_cfg import *

#RunOnData()#,['All'])
#process.patJetCorrFactors.corrSample = cms.string("Summer09_7TeV_ReReco332")
#process.patJetCorrFactors.corrSample = cms.string("Spring10")
# define path 'p': PAT Layer 0, PAT Layer 1, and the analyzer
process.p = cms.Path(process.primaryVertexFilter*
  
                     process.skimHLTFilter*
                     process.patDefaultSequence*
                     process.shNtupliser)


##process.source.fileNames = cms.untracked.vstring(
##"/store/data/Run2011A/Photon/AOD/PromptReco-v4/000/165/364/F208C043-BD84-E011-8207-001D09F24D67.root",
##"/store/data/Run2011A/Photon/AOD/PromptReco-v4/000/165/364/F8BD0083-B484-E011-8882-001D09F254CE.root",
##"/store/data/Run2011A/Photon/AOD/PromptReco-v4/000/165/364/A009987F-B484-E011-B7FB-001617E30D0A.root",
##"/store/data/Run2011A/Photon/AOD/PromptReco-v4/000/165/364/B23B7096-AE84-E011-A6FD-003048F1BF66.root",


##    )

##process.source.fileNames = cms.untracked.vstring(
##"/store/data/Run2011A/Photon/AOD/PromptReco-v4/000/165/364/F4C2BD4B-BA84-E011-936B-001D09F28EC1.root",
##"/store/data/Run2011A/Photon/AOD/PromptReco-v4/000/165/364/D0C28297-8784-E011-BAEC-00304879FBB2.root",
##"/store/data/Run2011A/Photon/AOD/PromptReco-v4/000/165/364/1884D371-B784-E011-8405-0030487C60AE.root",
##"/store/data/Run2011A/Photon/AOD/PromptReco-v4/000/165/364/94CE9A79-7784-E011-B828-0030487CD7EE.root"
##)
