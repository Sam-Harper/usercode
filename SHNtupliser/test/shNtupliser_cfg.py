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

process.GlobalTag.globaltag = cms.string('GR10_P_V9::All')

process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

#to allow me to re-run the tracking to redo electron seeds without H/E
process.load("RecoLocalTracker.SiPixelRecHits.SiPixelRecHits_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitConverter_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitMatcher_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.StripCPEfromTrackAngle_cfi")
process.load("RecoLocalTracker.SiStripZeroSuppression.SiStripZeroSuppression_cfi")
process.load("RecoLocalTracker.SiStripClusterizer.SiStripClusterizer_cfi")
process.load("RecoLocalTracker.SiPixelClusterizer.SiPixelClusterizer_cfi")
process.load("RecoLocalTracker.SiPixelRecHits.PixelCPEESProducers_cff")
process.load("RecoTracker.TransientTrackingRecHit.TTRHBuilders_cff")
process.load("RecoTracker.Configuration.RecoTracker_cff")
process.load("RecoTracker.IterativeTracking.FirstStep_cff")
process.load("SHarper.SHNtupliser.eleModulesNoHE_cfi")

import sys
process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = 1
process.shNtupliser.sampleWeight = 1
process.shNtupliser.gsfEleTag = "gsfElectrons"
process.shNtupliser.addMet = True
process.shNtupliser.addJets = True
process.shNtupliser.addMuons = True
process.shNtupliser.minEtToPromoteSC = 9;
process.shNtupliser.fillFromGsfEle = True
process.shNtupliser.minSCEtToPass = cms.double(-1)

hltProcName = "HLT"
process.shNtupliser.compTwoMenus = cms.bool(False)
process.shNtupliser.secondHLTTag = cms.string("")
process.shNtupliser.outputGeom = cms.bool(False)
process.shNtupliser.useHLTDebug = cms.bool(False)
process.shNtupliser.hltProcName = hltProcName
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","",hltProcName)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","",hltProcName)

process.shNtupliser.hltIsoEleProducer = cms.InputTag("hltPixelMatchElectronsL1Iso","",hltProcName)
process.shNtupliser.hltNonIsoEleProducer = cms.InputTag("hltPixelMatchElectronsL1NonIso","",hltProcName)
process.shNtupliser.hltIsoEcalCandProducer = cms.InputTag("hltL1IsoRecoEcalCandidate","",hltProcName)
process.shNtupliser.hltNonIsoEcalCandProducer = cms.InputTag("hltL1NonIsoRecoEcalCandidate","",hltProcName)
    
process.shNtupliser.ecalCandValuesToSave = cms.vstring("clusShape:hltL1IsoHLTClusterShape:hltL1NonIsoHLTClusterShape",
                                                       "hadNrgy:hltL1IsolatedPhotonHcalForHE:hltL1NonIsolatedPhotonHcalForHE",
                                                       "isolHad:hltL1IsolatedPhotonHcalIsol:hltL1NonIsolatedPhotonHcalIsol",
                                                       "isolPtTrks:hltL1IsoPhotonHollowTrackIsol:hltL1NonIsoPhotonHollowTrackIsol",
                                                       "isolPtTrksPt10DR03:hltL1IsoPhotonHollowTrackIsolPt10DR03:hltL1NonIsoPhotonHollowTrackIsolPt10DR03",
                                                       "isolPtTrksPt10DR06:hltL1IsoPhotonHollowTrackIsolPt10DR06:hltL1NonIsoPhotonHollowTrackIsolPt10DR06",                                                                                                             
                                                       "isolEm:hltL1IsolatedPhotonEcalIsol:hltL1NonIsolatedPhotonEcalIsol",
                                                       "isolEmWW:hltL1IsolatedPhotonEcalIsolJur:hltL1NonIsolatedPhotonEcalIsolJur")

process.shNtupliser.eleValuesToSave = cms.vstring("dPhiIn:hltElectronL1IsoDetaDphi@Dphi:hltElectronL1NonIsoDetaDphi@Dphi",
                                                  "dEtaIn:hltElectronL1IsoDetaDphi@Deta:hltElectronL1NonIsoDetaDphi@Deta",
                                                  "isolPtTrksEle:hltL1IsoElectronTrackIsol:hltL1NonIsoElectronTrackIsol",
#                                                  "isolPtTrksEleDR6:hltL1IsoElectronTrackIsolDR6:hltL1NonIsoElectronTrackIsolDR6",
                                                  "isolPtTrksElePt07:hltL1IsoElectronTrackIsolPt07:hltL1NonIsoElectronTrackIsolPt07")
#                                                  "isolPtTrksElePt07DEta03:hltL1IsoElectronTrackIsolPt07DEta03:hltL1NonIsoElectronTrackIsolPt07DEta03",
#                                                  "isolPtTrksElePt10DEta03:hltL1IsoElectronTrackIsolPt10DEta03:hltL1NonIsoElectronTrackIsolPt10DEta03")

#process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","","HLT")

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
    process.shNtupliser.outputFilename= sys.argv[3]
    process.shNtupliser.datasetCode = 1
    process.shNtupliser.sampleWeight = 1


process.shNtupliser.hltDebugFiltersToSave = cms.vstring(
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EleIdL1MatchFilterRegional", #0x1
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EleIdEtFilter", #0x2
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EleIdR9ShapeFilter", #0x4
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EleIdClusterShapeFilter", #0x8
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EleIdHcalIsolFilter", #0x10
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EleIdPixelMatchFilter", #0x20
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EleIdOneOEMinusOneOPFilter", #0x40
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EleIdDetaFilter", #0x80
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EleIdDphiFilter", #0x100
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10L1MatchFilterRegional", #0x200
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10EtFilter", #0x400
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10R9ShapeFilter", #0x800
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10HcalIsolFilter", #x1000
"hltL1NonIsoHLTNonIsoSingleElectronLWEt10PixelMatchFilter", #0x2000
"hltL1NonIsoHLTNonIsoSinglePhotonEt15L1MatchFilterRegional",
"hltL1NonIsoHLTNonIsoSinglePhotonEt15EtFilter",
"hltL1NonIsoSinglePhotonEt15HTITrackIsolFilter",
"hltL1NonIsoSinglePhotonEt15LEIEcalIsolFilter",
"hltL1NonIsoHLTNonIsoSingleElectronEt10L1MatchFilterRegional", #0x1
"hltL1NonIsoHLTNonIsoSingleElectronEt10EtFilter", #0x2
"hltL1NonIsoHLTNonIsoSingleElectronEt10HcalIsolFilter", #0x10
"hltL1NonIsoHLTNonIsoSingleElectronEt10PixelMatchFilter", #0x20
"hltL1sL1SingleEG5",
"hltL1sL1SingleEG8",
"hltEcalActivitySuperClusterWrapper", #0x400000
"hltEgammaSelectEcalSuperClustersActivityFilter" #0x800000

)

filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""


process.source = cms.Source("PoolSource",
                          fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
)

process.myPref = cms.ESPrefer("PoolDBESSource","GlobalTag")

process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                                      vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                                      minimumNDOF = cms.uint32(4) ,
 						      maxAbsZ = cms.double(15),	
 						      maxd0 = cms.double(2)	
                                                      )


process.load("JetMETCorrections.Configuration.JetCorrectionCondDB_cff")

#no configuration is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");

runningOn35XSample=False
#the below comment is really really important, my scripts reset this
#CRAB35XSAMPLEOVERWRITE

if runningOn35XSample: 
#for 3_5_6 samples
    process.patJets.discriminatorSources = cms.VInputTag(
    cms.InputTag("combinedSecondaryVertexBJetTags"),
    cms.InputTag("combinedSecondaryVertexMVABJetTags"),
    cms.InputTag("jetBProbabilityBJetTags"),
    cms.InputTag("jetProbabilityBJetTags"),
    cms.InputTag("simpleSecondaryVertexBJetTags"),
    #cms.InputTag("simpleSecondaryVertexNegativeBJetTags"),
    cms.InputTag("softElectronByPtBJetTags"),                
    cms.InputTag("softElectronByIP3dBJetTags"),
    cms.InputTag("softMuonBJetTags"),
    cms.InputTag("softMuonByPtBJetTags"),                
    cms.InputTag("softMuonByIP3dBJetTags"),
    cms.InputTag("trackCountingHighEffBJetTags"),
    cms.InputTag("trackCountingHighPurBJetTags"),
    )


from PhysicsTools.PatAlgos.tools.coreTools import *
removeMCMatching(process,['All'])
#process.patJetCorrFactors.corrSample = cms.string("Summer09_7TeV_ReReco332")
process.patJetCorrFactors.corrSample = cms.string("Spring10")
# define path 'p': PAT Layer 0, PAT Layer 1, and the analyzer
process.p = cms.Path(process.primaryVertexFilter*
    #process.siPixelRecHits*
                     #process.siStripMatchedRecHits*
                     #process.firstStep*
                     #process.newCombinedSeeds*
                     #process.ecalDrivenElectronSeedsNoHE*
                    # process.generalTracks*
                    # process.trackerDrivenElectronSeeds*
                     #process.electronMergedSeedsNoHE* 
                  #   process.electronCkfTrackCandidatesNoHE*
                  #  process.electronGsfTracksNoHE*
                   #  process.gsfElectronCoresNoHE*
                   #  process.gsfElectronsNoHE*
                     process.patDefaultSequence*
                     process.shNtupliser)


