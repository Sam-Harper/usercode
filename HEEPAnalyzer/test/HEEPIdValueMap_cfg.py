# Import configurations
import FWCore.ParameterSet.Config as cms


# set up process
process = cms.Process("HEEP")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(1000),
    limit = cms.untracked.int32(10000000)
)

process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond['run2_mc'] 


#
# Define input data to read
#
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring(
  'file:/home/ppd/mjf21517/scratch/mcTestFiles/ZToEE_NNPDF30_13TeV-powheg_M_200_400_PUSpring16_80X_mcRun2_asymptotic_2016_v3-v2_FE668C0A-8B09-E611-ACDE-B083FED76DBD.root',
  '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/0015C81C-370A-E611-8073-0CC47A01CAFC.root',
),
                             eventsToProcess = cms.untracked.VEventRange("1:2862883-1:2862883")
                             
)
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)

#setting up the producer to make the HEEP ID value map
from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *
process.HEEPId = cms.EDProducer("HEEPIdValueMapProducer",
                                eleLabel = cms.InputTag("gedGsfElectrons"),
                                barrelCuts = cms.PSet(heepBarrelCuts), #for V6.1 (miniAOD compromise) do heeepBarrelCutsV61
                                endcapCuts = cms.PSet(heepEndcapCuts) ,#for V6.1 (miniAOD compromise) do heeepBarrelCutsV61
                                trkIsoNoJetCoreTag = cms.InputTag(""),
                                nrSatCrysIn5x5Tag = cms.InputTag(""),
                                eleRhoCorrTag = cms.InputTag("fixedGridRhoFastjetAll"),
                                verticesTag = cms.InputTag("offlinePrimaryVertices"),
                                beamSpotTag = cms.InputTag("offlineBeamSpot"),
                                reducedBarrelRecHitTag = cms.InputTag("reducedEcalRecHitsEB"),
                                reducedEndcapRecHitTag = cms.InputTag("reducedEcalRecHitsEE"),
                                ctfTrackTag=cms.InputTag("generalTracks"),
                                trkIsoConfig= cms.PSet(
                                   extRadius=cms.double(0.3),
                                   intRadiusBarrel=cms.double(0.015),
                                   intRadiusEndcap=cms.double(0.015),
                                   stripBarrel=cms.double(0.015),
                                   stripEndcap=cms.double(0.015),
                                   ptMin=cms.double(0.7),
                                   maxVtxDist=cms.double(0.2),
                                   drb=cms.double(999999999.0)
                                ),
                                writeIdAsInt =cms.bool(True) #true saves the heep ID as an int, false: saves as a float, user request
                                )



process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)
import sys
#this is a safety to stop the unwary deleteing their input file ;)
if len(sys.argv)>2:
    process.TFileService.fileName = cms.string(sys.argv[len(sys.argv)-1])




process.p = cms.Path(process.HEEPId #makes the HEEPID value map
                     
                     ) 
