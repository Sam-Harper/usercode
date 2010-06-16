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
process.GlobalTag.globaltag = cms.string('GR10_P_V5::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
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
process.shNtupliser.minEtToPromoteSC = 7;
process.shNtupliser.fillFromGsfEle = True
process.shNtupliser.minSCEtToPass = cms.double(-1)

process.shNtupliser.outputGeom = cms.bool(False)
process.shNtupliser.useHLTDebug = cms.bool(False)
process.shNtupliser.hltProcName = "HLT"
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","","HLT")
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

process.source = cms.Source("PoolSource",
                      #      fileNames = cms.untracked.vstring("dcap://heplnx209.pp.rl.ac.uk:22125"+sys.argv[2]),
                          fileNames = cms.untracked.vstring(sys.argv[2]),
                         #   fileNames = cms.untracked.vstring("file:"+sys.argv[2]),
                            )


process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                                      vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                                      minimumNDOF = cms.uint32(4) ,
 						      maxAbsZ = cms.double(15),	
 						      maxd0 = cms.double(2)	
                                                      )



#no configuration is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff");

runningOn35XSample=True
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

##process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange(
##	'132440:85-132440:138',
##	'132440:141-132440:401',
##	'132473:1-132473:29',
##	'132476:23-132476:28',
##	'132476:54-132476:57',
##	'132477:1-132477:5',
##	'132477:34-132477:35',
##	'132477:63-132477:64',
##	'132477:90-132477:93',
##	'132477:118-132477:121',
##	'132477:148-132477:149',
##	'132477:176-132477:179',
##	'132477:225-132477:236',
##	'132477:368-132477:384',
##	'132477:517-132477:520',
##	'132596:382-132596:453',
##	'132598:80-132598:82',
##	'132598:174-132598:188',
##	'132599:1-132599:379',
##	'132599:381-132599:538',
##	'132601:1-132601:207',
##	'132601:209-132601:259',
##	'132601:261-132601:1131',
##	'132602:1-132602:83',
##	'132605:1-132605:444',
##	'132605:446-132605:622',
##	'132605:624-132605:829',
##	'132605:831-132605:968',
##	'132606:1-132606:37',
##	'132656:1-132656:140',
##	'132658:1-132658:177',
##	'132659:1-132659:84',
##	'132661:1-132661:130',
##	'132662:1-132662:130',
##	'132662:132-132662:217',
##	'132716:220-132716:591',
##	'132716:593-132716:640',
##	'132959:1-132959:276',
##	'132959:278-132959:417',
##	'132960:1-132960:190',
##	'132961:1-132961:427',
##	'132965:1-132965:107',
##	'132968:1-132968:173',
##	'133029:101-133029:115',
##	'133029:129-133029:350',
##	'133031:1-133031:18',
##	'133034:131-133034:325',
##	'133035:1-133035:306',
##	'133036:1-133036:225',
##	'133046:1-133046:43',
##	'133046:45-133046:323',
##	'133158:65-133158:786',
##	'133874:166-133874:814',
##	'133874:817-133874:875',
##	'133875:1-133875:20',
##	'133875:22-133875:49',
##	'133876:1-133876:330',
##	'133877:1-133877:1640',
##	'133877:1643-133877:1997',
##	'133881:1-133881:71',
##	'133881:74-133881:223',
##	'133881:225-133881:562',
##	'133885:1-133885:132',
##	'133885:134-133885:728',
##	'133927:1-133927:57',
##	'133928:1-133928:645',
##	'134721:294-134721:468',
##	'134721:470-134721:677',
##	'134721:679-134721:1340',
##	'134725:231-134725:565',
##	'134725:567-134725:1315',
##	'135059:59-135059:67',
##	'135149:294-135149:337',
##	'135149:339-135149:754',
##	'135149:756-135149:932',
##	'135149:934-135149:1808',
##	'135149:1811-135149:3382',
##	'135175:55-135175:561',
##	'135175:563-135175:790',
##	'135175:792-135175:1082',
##	'135445:997-135445:1329',
##	'135445:1332-135445:1827',
##	'135521:60-135521:440',
##	'135521:442-135521:524',
##	'135523:1-135523:124',
##	'135523:126-135523:225',
##	'135525:1-135525:3',
##	'135525:6-135525:457',
##	'135534:83-135534:90',
##	'135535:75-135535:167',
##	'135535:169-135535:246',
##	'135575:2-135575:210',
##	'135575:213-135575:381',
##	'135575:384-135575:638',
##	'135575:645-135575:1161',
##	'135575:1163-135575:1253',
##	'135735:31-135735:333',
##)
