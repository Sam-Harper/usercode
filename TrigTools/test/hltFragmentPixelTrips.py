
#>>>From hltFragment <<<#
from SHarper.TrigTools.hltConfigTools import *
#process.maxEvents.input=-1
rmAllEndPathsWithOutput(process)
addOutputMod(process)

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis')
options.parseArguments()

isCrabJob=False #script seds this if its a crab job
#if 1, its a crab job...
if isCrabJob:
    process.hltOutputTot.fileName=cms.untracked.string("OUTPUTFILE")
else:
    process.hltOutputTot.fileName=cms.untracked.string(options.outputFile)

process.MessageLogger.suppressWarning.extend(["hltEgammaGsfTracks","hltEgammaGsfTracksUnseeded","hltIter2ElectronsCtfWithMaterialTracks"])
process.MessageLogger.suppressInfo.extend(["hltEgammaGsfTracks","hltEgammaGsfTracksUnseeded","hltIter2ElectronsCtfWithMaterialTracks"])
process.nrEventsStorer = cms.EDProducer("NrInputEventsStorer")
process.HLTriggerFirstPath.insert(0,process.nrEventsStorer)
#process.hltOutputTot.SelectEvents.SelectEvents = cms.vstring("HLT_Ele27_WPTight_Gsf_NoPM_v1","HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_NoPM_v1","HLT_DoubleEle33_CaloIdL_NoPM_v1")
#process.hltOutputTot.outputCommands = cms.untracked.vstring("keep *",)

#process.hltOutputTot.outputCommands = outputCmdsRePixelMediumWithGen()                                            
process.hltOutputTot.eventAutoFlushCompressedSize = cms.untracked.int32(5*1024*1024)
print  "global tag : ",process.GlobalTag.globaltag

process.hltEgammaSuperClustersToPixelMatch = cms.EDProducer("EgammaHLTFilteredSuperClusterProducer",
                                                            cands = cms.InputTag("hltEgammaCandidates"),
                                                            cuts = cms.VPSet(
        cms.PSet(
            var = cms.InputTag("hltEgammaHoverE"),
            barrelCut = cms.PSet(
                cutOverE = cms.double(0.2),
                useEt = cms.bool(False)
                ),
            endcapCut = cms.PSet(
                cutOverE = cms.double(0.2),
                useEt = cms.bool(False)
            )
            )
        )
                                                            )

process.hltEleSeedsTrackingRegions = cms.EDProducer("TrackingRegionsFromSuperClustersEDProducer",
                                                  RegionPSet = cms.PSet( 
        ptMin=cms.double(1.5),
        originRadius=cms.double(0.2,),
        originHalfLength=cms.double(15.0),
        deltaPhiRegion=cms.double(0.4),
        deltaEtaRegion=cms.double(0.1),
        useZInVertex=cms.bool(False),
        whereToUseMeasTracker = cms.string("kNever"),
        beamSpot = cms.InputTag("hltOnlineBeamSpot" ),
        vertices = cms.InputTag(""),
        measurementTrackerEvent = cms.InputTag(""),
        superClusters=cms.VInputTag('hltEgammaSuperClustersToPixelMatch'),
        precise = cms.bool( True )
        )
)


process.hltEgammaElectronPixelSeeds = cms.EDProducer("ElectronNSeedProducer",
                                                     initialSeeds = cms.InputTag("hltElePixelSeedsCombined"),
                                                     vertices = cms.InputTag(""),
                                                     navSchool = cms.string("SimpleNavigationSchool"),
                                                     detLayerGeom = cms.string("hltESPGlobalDetLayerGeometry"),
                                                     beamSpot = cms.InputTag("hltOnlineBeamSpot"),
                                                     measTkEvt = cms.InputTag( "hltSiStripClusters" ),
                                                     superClusters=cms.VInputTag('hltEgammaSuperClustersToPixelMatch'),                                                    
                                                     useRecoVertex=cms.bool(False),
                                                     matchingCuts=cms.VPSet(
        cms.PSet(dPhiMax=cms.double(0.08),
                 dZMax=cms.double(999999),
                 dRIMax=cms.double(999999),
                 dRFMax=cms.double(999999)
                 ),
        cms.PSet(dPhiMax=cms.double(0.004),
                 dZMax=cms.double(0.09),
                 dRIMax=cms.double(0.09),
                 dRFMax=cms.double(0.09)
                 ),
        cms.PSet(dPhiMax=cms.double(0.004),
                 dZMax=cms.double(0.09),
                 dRIMax=cms.double(0.09),
                 dRFMax=cms.double(0.09)
                 ),
        )
)
relaxPMCutsForStudies=True

if relaxPMCutsForStudies:
    process.hltEgammaElectronPixelSeeds.matchingCuts=cms.VPSet(
        cms.PSet(dPhiMax=cms.double(0.2),
                 dZMax=cms.double(999999),
                 dRIMax=cms.double(999999),
                 dRFMax=cms.double(999999)
                 ),
        cms.PSet(dPhiMax=cms.double(0.2),
                 dZMax=cms.double(0.2),
                 dRIMax=cms.double(0.2),
                 dRFMax=cms.double(0.15)
                 ),
        cms.PSet(dPhiMax=cms.double(0.2),
                 dZMax=cms.double(0.2),
                 dRIMax=cms.double(0.2),
                 dRFMax=cms.double(0.2)
                 )
        )
                                             
process.hltEgammaPixelMatchVars.productsToWrite = cms.int32( 2 )
