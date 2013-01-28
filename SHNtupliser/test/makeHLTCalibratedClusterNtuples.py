import sys

# Import configurations
import FWCore.ParameterSet.Config as cms

outputReco=False

# set up process
process = cms.Process("USER")

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

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)




process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")
process.load("RecoLocalCalo.EcalRecAlgos.EcalSeverityLevelESProducer_cfi")


dataset="Photon"
#DATASETSOVERWRITE
datasetVersion="13Jul"
#DATASETVERSIONOVERWRITE

if datasetVersion.find("13Jul")!=-1:
    process.GlobalTag.globaltag = "FT_53_V6_AN2::All"
elif datasetVersion.find("Aug06")!=-1:
    process.GlobalTag.globaltag = "FT_53_V6_AN2::All"
elif datasetVersion.find("24Aug")!=-1:
    process.GlobalTag.globaltag = "FT_53_V6_AN2::All"   
else:
    process.GlobalTag.globaltag = "GR_P_V42_AN2::All"

print "Global Tag is ",process.GlobalTag.globaltag 



filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""


if(sys.argv[2].find("/castor/")==0):
    filePrefex="rfio:"
process.source = cms.Source("PoolSource",
                          #fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                           fileNames = cms.untracked.vstring(),
                         #   inputCommands = cms.untracked.vstring("drop *_*_*_RECO","keep *_*BeamSpot*_*_*","keep *_muons_*_*"),
                            eventsToProcess =cms.untracked.VEventRange()
                         
)

for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])


process.deCalibRecHits = cms.EDProducer("DeCalibEcalRecHitProducer",
                                        inputEBRecHitTag=cms.InputTag("reducedEcalRecHitsEB"),
                                        inputEERecHitTag=cms.InputTag("reducedEcalRecHitsEE"),
                                        doLaser=cms.bool(True),
                                        doInterCalib=cms.bool(False),
                                        deCalib=cms.bool(True),
                                        ebLaserMin=cms.double(0.5),
                                        ebLaserMax=cms.double(2),
                                        eeLaserMin=cms.double(0.5),
                                        eeLaserMax=cms.double(3)
                                        
                                      )




#process.myPref = cms.ESPrefer("PoolDBESSource","GlobalTag")
process.load("SHarper.HEEPAnalyzer.HEEPFilter_cfi")

process.load("SHarper.SHNtupliser.deCalibratedClusters_cfi")

process.hltHybridSuperClustersActivity.ecalhitproducer="deCalibRecHits"
process.hltCorrectedHybridSuperClustersActivity.recHitProducer = cms.InputTag( 'deCalibRecHits','EcalRecHitsEB' )
process.hltMulti5x5BasicClustersActivity.endcapHitProducer="deCalibRecHits"
process.hltMulti5x5SuperClustersWithPreshowerActivity.preshRecHitProducer = cms.InputTag('reducedEcalRecHitsES')
process.hltCorrectedMulti5x5SuperClustersWithPreshowerActivity.recHitProducer=cms.InputTag('deCalibRecHits','EcalRecHitsEE' )

hltName="HLT"
patCandID=""
process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = 1
process.shNtupliser.sampleWeight = 1
process.shNtupliser.gsfEleTag = "gsfElectrons"
process.shNtupliser.addMet = False
process.shNtupliser.addJets = False
process.shNtupliser.addMuons = False
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


import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Ele32_CaloIdT_CaloIsoT_TrkIdT_TrkIsoT_SC17_Mass50_v*")
process.skimHLTFilter.throw = False


process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                                      vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                                      minimumNDOF = cms.uint32(4) ,
 						      maxAbsZ = cms.double(24),	
 						      maxd0 = cms.double(2)	
                                                      )

process.p = cms.Path(process.skimHLTFilter*process.primaryVertexFilter*process.heepFilter*process.deCalibRecHits*process.HLTBarrelClusterSequence*process.HLTEndcapClusterSequence*process.shNtupliser)

process.load('Configuration/EventContent/EventContent_cff')

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)

isCrabJob=False #script seds this if its a crab job

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    #process.shNtupliser.outputFilename= "OUTPUTFILE"
    process.TFileService.fileName= "OUTPUTFILE"
    process.shNtupliser.datasetCode = DATASETCODE
    #process.shNtupliser.sampleWeight = SAMPLEWEIGHT
    process.shNtupliser.sampleWeight = 1
else:
    print "using user specified filename"
    #process.shNtupliser.outputFilename= sys.argv[len(sys.argv)-1]
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]
    process.shNtupliser.datasetCode = -1
    process.shNtupliser.sampleWeight = 1


##import PhysicsTools.PythonAnalysis.LumiList as LumiList
##import FWCore.ParameterSet.Types as CfgTypes
##myLumis = LumiList.LumiList(filename = 'goodList.json').getCMSSWString().split(',')
##process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
##process.source.lumisToProcess.extend(myLumis)



