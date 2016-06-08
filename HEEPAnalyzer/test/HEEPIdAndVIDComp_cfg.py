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
#process.GlobalTag.globaltag = autoCond['run2_data']

#
# Define input data to read
#
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring(
  'file:/home/ppd/mjf21517/scratch/mcTestFiles/ZToEE_NNPDF30_13TeV-powheg_M_200_400_PUSpring16_80X_mcRun2_asymptotic_2016_v3-v2_FE668C0A-8B09-E611-ACDE-B083FED76DBD.root',
  '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/0015C81C-370A-E611-8073-0CC47A01CAFC.root',
##  '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/08717217-7A09-E611-AAC6-44A84225CABC.root',
 # '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/22E2313E-620A-E611-AEAB-20CF3019DF10.root',
 # '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/36280953-620A-E611-9B77-6C3BE5B5F228.root',
 # '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/6240D9B6-350A-E611-9661-A0369F7FC714.root',
 # '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/6A28658B-9009-E611-8DD6-001E67504B25.root',
 # '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/B44BBCEF-750A-E611-806E-FA163EA6F15D.root',
 # '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/BCEAC9C3-7A09-E611-ABCB-6C3BE5B58198.root',
 # '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/E2C3843B-610A-E611-B964-20CF3019DF00.root',
 # '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/F0BBAF9F-350A-E611-8063-0090FAA57E94.root',
 # '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf/AODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/50000/F272EEA2-9009-E611-B6A0-B083FED0FFCF.root',

 # 'file:/home/ppd/mjf21517/scratch/mcTestFiles/ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf_PUSpring16_80X_mcRun2_asymptotic_2016_v3-v2_6240D9B6-350A-E611-9661-A0369F7FC714.root',
),
              #               eventsToProcess = cms.untracked.VEventRange("1:2862883-1:2862883")
                             
)
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)

#setting up the producer to make the HEEP ID value map
from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *
process.heepId = cms.EDProducer("HEEPIdValueMapProducer",
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


from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format  to be
# DataFormat.AOD or DataFormat.MiniAOD, as appropriate
useAOD=True
if useAOD:
    dataFormat = DataFormat.AOD
else :
    dataFormat = DataFormat.MiniAOD

switchOnVIDElectronIdProducer(process, dataFormat)

# define which IDs we want to produce
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV60_80XAOD_cff',
                 'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV61_cff']

#add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)


process.heepIdVIDComp = cms.EDAnalyzer("HEEPIdAndVIDComp",
                                       eleLabel=cms.InputTag("gedGsfElectrons"),
                                       genPartLabel=cms.InputTag("genParticles"),
                                       heepId=cms.InputTag("heepId"),
                                       vid=cms.InputTag("egmGsfElectronIDs:heepElectronID-HEEPV60-80XAOD")
#                                       vid=cms.InputTag("egmGsfElectronIDs:heepElectronID-HEEPV61")
                                       )


if useAOD==False:
    process.heepId.verticesLabel=cms.InputTag("offlineSlimmedPrimaryVertices")
    process.heepId.eleLabel=cms.InputTag("slimmedElectrons")
    process.heepIdVIDComp.eleLabel=cms.InputTag("slimmedElectrons")
    

process.heepIDVarValueMaps = cms.EDProducer("ElectronHEEPIDValueMapProducer",
                                            ebRecHits=cms.InputTag("reducedEcalRecHitsEB"),
                                            eeRecHits=cms.InputTag("reducedEcalRecHitsEB"),
                                            beamSpot=cms.InputTag("offlineBeamSpot"),
                                            tracks=cms.InputTag("generalTracks"),
                                            eles=cms.InputTag("gedGsfElectrons"),
                                            trkIsoConfig= cms.PSet(
                                               extRadius=cms.double(0.3),
                                               intRadiusBarrel=cms.double(0.015),
                                               intRadiusEndcap=cms.double(0.015),
                                               stripBarrel=cms.double(0.015),
                                               stripEndcap=cms.double(0.015),
                                               ptMin=cms.double(0.7),
                                               maxVtxDist=cms.double(0.2),
                                               drb=cms.double(999999999.0)
                                               )
                                            )
                                            

process.p = cms.Path(process.heepId* #makes the HEEPID value map
                     process.heepIDVarValueMaps*
                     process.egmGsfElectronIDSequence* #makes the VID value maps
                     process.heepIdVIDComp
                     )

