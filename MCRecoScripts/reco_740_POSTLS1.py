# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step3 --conditions auto:run2_mc -n 10 --eventcontent AODSIM -s RAW2DIGI,L1Reco,RECO,EI,VALIDATION,DQM --datatier AODSIM --customise SLHCUpgradeSimulations/Configuration/postLS1Customs.customisePostLS1 --magField 38T_PostLS1 --io RECOUP15_PU25.io --python RECOUP15_PU25.py --conditions=MCRUN2_73_V7 --no_exec --filein file:step2.root --fileout file:step3.root
import FWCore.ParameterSet.Config as cms

process = cms.Process('RECO')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('CommonTools.ParticleFlow.EITopPAG_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)



process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
              #              eventsToProcess = cms.untracked.VEventRange("1:16731-1:16731")
)


isCrabJob=False #submiting script seds this to true

if not isCrabJob:
    
    import sys
    filePrefex="file:"
    if(sys.argv[3].find("/pnfs/")==0):
        filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"
        
    if(sys.argv[3].find("/store/")==0):
        filePrefex=""
    if(sys.argv[3].find("/eos/")==0):
        filePrefex="'root://eoscms/"
    for i in range(3,len(sys.argv)-1):
        print filePrefex+sys.argv[i]
        process.source.fileNames.extend([filePrefex+sys.argv[i],])
    
    


process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.19 $'),
    annotation = cms.untracked.string('step3 nevts:10'),
    name = cms.untracked.string('Applications')
)

# Output definition

process.AODSIMoutput = cms.OutputModule("PoolOutputModule",
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = process.RECOSIMEventContent.outputCommands,
    fileName = cms.untracked.string('TOSED:OUTPUTFILE'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('AODSIM')
    )
)
#process.AODSIMoutput.outputCommands.extend(cms.untracked.vstring("drop recoGenParticles_*_*_*","drop recoPFJets_*_*_*","drop triggerTriggerEvent_*_*_*","drop recoTrack*_*_*_*","drop recoCaloJets_*_*_*","drop *_generalTracks_*_*","drop recoJet*_*_*_*","drop *FwdPtrs*_*_*_*","drop recoGenJets*_*_*_*","drop ints_genParticles_*_*","drop recoPFTau*_*_*_*","drop recoDeDxData*_*_*_*","drop CastorRecHits*_*_*_*","drop recoCastorTowers_*_*_*","drop HFRecHitsSorted_*_*_*","drop TrackingRecHitsOwned_*_*_*","keep recoTracks_generalTracks_*_*","drop *_*ulti5x5*_*_*","drop recoJPTJets_*_*_*","drop *_*ybridSuperClusters_*_*","keep *_particleFlowClusterECAL_*_*","keep *_particleFlowClusterHCAL_*_*","drop *_muons_*_*","drop *_*onversions_*_*","drop recoRecoChargedRefCandidates_trackRefsForJets_*_*","drop recoPFRecHits_particleFlowRecHitHO_*_*","drop *_offlinePrimaryVerticesWithBS_*_*","keep *_particleFlowBlock_*_*" ))

if not isCrabJob:
    process.AODSIMoutput.fileName = cms.untracked.string(sys.argv[len(sys.argv)-1])
    
# Additional output definition

# Other statements

# Other statements
process.mix.playback = True
process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)
process.RandomNumberGeneratorService.restoreStateLabel=cms.untracked.string("randomEngineStateProducer")


from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag as customiseGlobalTag



if isCrabJob:
  
    process.GlobalTag = customiseGlobalTag(process.GlobalTag,globaltag= 'TOSED:GLOBALTAG')
else:
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag=sys.argv[2])

#multi fit needs to know the bunch spacing
#however the pileup info producer does not produce this in 72X
#so if we are running >=73X on 72X, we need to manual
process.ecalMultiFitUncalibRecHit.algoPSet.useLumiInfoRunHeader=False
process.particleFlowClusterECAL.energyCorrector.autoDetectBunchSpacing=False
process.particleFlowClusterECAL.energyCorrector.bunchSpacing=cms.int32(25)

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstruction)
process.eventinterpretaion_step = cms.Path(process.EIsequence)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.AODSIMoutput_step = cms.EndPath(process.AODSIMoutput)

applyEGFilter=False
if applyEGFilter:
    process.egammaFilter = cms.EDFilter("EGammaFilter",
                                        nrElesRequired=cms.int32(-1),
                                        nrPhosRequired=cms.int32(-1),
                                        nrSCsRequired=cms.int32(-1),
                                        eleEtCut=cms.double(-1),
                                        phoEtCut=cms.double(-1),
                                        scEtCut=cms.double(30),
                                        eleTag=cms.InputTag("gedGsfElectrons"),
                                        phoTag=cms.InputTag("gedPhotons"),
                                        superClusEBTag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALBarrel"),
                                        superClusEETag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALEndcapWithPreshower"),
                                        caloTowerTag = cms.InputTag("towerMaker"),
                                        requireEcalDriven=cms.bool(False)
                                        )

    process.caloTowersRec.insert(1,process.egammaFilter)
    process.eventinterpretaion_step.insert(0,process.egammaFilter)
    
# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.eventinterpretaion_step,process.endjob_step,process.AODSIMoutput_step)

# customisation of the process.

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.postLS1Customs
from SLHCUpgradeSimulations.Configuration.postLS1Customs import customisePostLS1 

#call to customisation function customisePostLS1 imported from SLHCUpgradeSimulations.Configuration.postLS1Customs
process = customisePostLS1(process)

# Automatic addition of the customisation function from SimGeneral.MixingModule.fullMixCustomize_cff
from SimGeneral.MixingModule.fullMixCustomize_cff import setCrossingFrameOn 

#call to customisation function setCrossingFrameOn imported from SimGeneral.MixingModule.fullMixCustomize_cff
process = setCrossingFrameOn(process)

# End of customisation functions


#process.MessageLogger.suppressWarning.append("caloStage1Digis")
#process.MessageLogger.suppressInfo.append("caloStage1Digis")

process.MessageLogger.suppressError=cms.untracked.vstring("caloStage1Digis",)
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
##from CondCore.DBCommon.CondDBSetup_cfi import *
##process.ecalES1 = cms.ESSource("PoolDBESSource",CondDBSetup,
##                                        connect = cms.string("frontier://FrontierProd/CMS_COND_31X_ECAL"),
##                                        toGet = cms.VPSet(
##                                            cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),tag=cms.string("EcalIntercalib_2011_V3_Bon_mc_subModCor_smeared_1fb")),
                                           
##                                            cms.PSet(record = cms.string("EcalPedestalsRcd"),tag=cms.string("EcalPedestals_208944_200_mc")),
##                                                          )
##                                        )

##process.es_prefer_ecal1 = cms.ESPrefer("PoolDBESSource","ecalES1")
