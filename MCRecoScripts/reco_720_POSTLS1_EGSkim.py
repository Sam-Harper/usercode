# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step3 --customise SLHCUpgradeSimulations/Configuration/postLS1Customs.customisePostLS1 --conditions auto:run2_mc -s RAW2DIGI,L1Reco,RECO,EI,VALIDATION,DQM --datatier GEN-SIM-RECO,DQM -n 10 --magField 38T_PostLS1 --eventcontent RECOSIM,DQM --io RECOUP15.io --python RECOUP15.py --no_exec --filein file:step2.root --fileout file:step3.root
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
    input = cms.untracked.int32(200)
)

import sys
filePrefex="file:"
if(sys.argv[3].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[3].find("/store/")==0):
    filePrefex=""
if(sys.argv[3].find("/eos/")==0):
    filePrefex="'root://eoscms/"

process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
              #              eventsToProcess = cms.untracked.VEventRange("1:16731-1:16731")
)


isCrabJob=False #submiting script seds this to true

if not isCrabJob:
    for i in range(3,len(sys.argv)-1):
        print filePrefex+sys.argv[i]
        process.source.fileNames.extend([filePrefex+sys.argv[i],])
    
    

process.options = cms.untracked.PSet(wantSummary= cms.untracked.bool(True))


# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.19 $'),
    annotation = cms.untracked.string('step3 nevts:10'),
    name = cms.untracked.string('Applications')
)

# Output definition

process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
                                         SelectEvents = cms.untracked.PSet(  SelectEvents = ( cms.vstring( 'reconstruction_step',))),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = process.AODSIMEventContent.outputCommands,
    fileName = cms.untracked.string('TOSED:OUTPUTFILE'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('GEN-SIM-RECO')
    )
)
process.RECOSIMoutput.outputCommands.extend(cms.untracked.vstring("drop recoGenParticles_*_*_*","drop recoPFJets_*_*_*","drop triggerTriggerEvent_*_*_*","drop recoTrack*_*_*_*","drop recoCaloJets_*_*_*","drop *_generalTracks_*_*","drop recoJet*_*_*_*","drop *FwdPtrs*_*_*_*","drop recoGenJets*_*_*_*","drop ints_genParticles_*_*","drop recoPFTau*_*_*_*","drop recoDeDxData*_*_*_*","drop CastorRecHits*_*_*_*","drop recoCastorTowers_*_*_*","drop HFRecHitsSorted_*_*_*","drop TrackingRecHitsOwned_*_*_*","keep recoTracks_generalTracks_*_*","drop *_*ulti5x5*_*_*","drop recoJPTJets_*_*_*","drop *_*ybridSuperClusters_*_*","keep *_particleFlowClusterECAL_*_*","keep *_particleFlowClusterHCAL_*_*","drop *_muons_*_*","drop *_*onversions_*_*","drop recoRecoChargedRefCandidates_trackRefsForJets_*_*","drop recoPFRecHits_particleFlowRecHitHO_*_*","drop *_offlinePrimaryVerticesWithBS_*_*","keep *_particleFlowBlock_*_*" ))
#process.RECOSIMoutput.outputCommands.append("keep *_particleFlowClusterECAL_*_*")
#process.RECOSIMoutput.outputCommands.append("keep *_particleFlowClusterHCAL_*_*")

if not isCrabJob:
    process.RECOSIMoutput.fileName = cms.untracked.string(sys.argv[len(sys.argv)-1])

# Additional output definition

# Other statements
process.mix.playback = True
process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)
process.RandomNumberGeneratorService.restoreStateLabel=cms.untracked.string("randomEngineStateProducer")
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag

if isCrabJob:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'TOSED:GLOBALTAG', '')
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, sys.argv[2], '')  


## process.ecalMultiFitUncalibRecHit== cms.EDProducer("EcalUncalibRecHitProducer",
##     EBdigiCollection = cms.InputTag("ecalDigis","ebDigis"),
##      EEhitCollection = cms.string('EcalUncalibRecHitsEE'),
##      EEdigiCollection = cms.InputTag("ecalDigis","eeDigis"),
##      EBhitCollection = cms.string('EcalUncalibRecHitsEB'),
##      algo = cms.string("EcalUncalibRecHitWorkerWeights")
##  )

process.mcFilter = cms.EDFilter("MCTruthFilter",
                                   genParticlesTag = cms.InputTag("genParticles"),
                                    pid=cms.int32(11)
                                )
process.egammaFilter = cms.EDFilter("EGammaFilter",
                                    nrElesRequired=cms.int32(-1),
                                    nrPhosRequired=cms.int32(-1),
                                    nrSCsRequired=cms.int32(1),
                                    eleEtCut=cms.double(-1),
                                    phoEtCut=cms.double(-1),
                                    scEtCut=cms.double(20),
                                    eleTag=cms.InputTag("gedGsfElectrons"),
                                    phoTag=cms.InputTag("gedPhotons"),
                                    superClusEBTag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALBarrel"),
                                    superClusEETag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALEndcapWithPreshower"),
                                    caloTowerTag = cms.InputTag("towerMaker"),
                                    requireEcalDriven=cms.bool(False)
                                     )
process.eleFilter = cms.EDFilter("EGammaFilter",
                                 nrElesRequired=cms.int32(1),
                                 nrPhosRequired=cms.int32(-1),
                                 nrSCsRequired=cms.int32(-1),
                                 eleEtCut=cms.double(20),
                                 phoEtCut=cms.double(-1),
                                 scEtCut=cms.double(-1),
                                 eleTag=cms.InputTag("gedGsfElectrons"),
                                 phoTag=cms.InputTag("gedPhotons"),
                                 superClusEBTag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALBarrel"),
                                 superClusEETag = cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALEndcapWithPreshower"),
                                 caloTowerTag = cms.InputTag("towerMaker"),
                                 requireEcalDriven=cms.bool(True)
                                 )

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstruction*process.eleFilter)
process.eventinterpretaion_step = cms.Path(process.egammaFilter*process.eleFilter*process.EIsequence)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

#process.gedElectronPFIsoSequence.insert(process.gedElectronPFIsoSequence.index(process.gedGsfElectrons)+1,process.egammaFilter)
process.caloTowersRec.insert(1,process.egammaFilter)


# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.eventinterpretaion_step,process.endjob_step,process.RECOSIMoutput_step)

# customisation of the process.

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.postLS1Customs
from SLHCUpgradeSimulations.Configuration.postLS1Customs import customisePostLS1 

#call to customisation function customisePostLS1 imported from SLHCUpgradeSimulations.Configuration.postLS1Customs
process = customisePostLS1(process)

# Automatic addition of the customisation function from SimGeneral.MixingModule.fullMixCustomize_cff
from SimGeneral.MixingModule.fullMixCustomize_cff import setCrossingFrameOn 

#call to customisation function setCrossingFrameOn imported from SimGeneral.MixingModule.fullMixCustomize_cff
process = setCrossingFrameOn(process)

print process.particleFlowClusterECAL.inputECAL

# End of customisation functions
if process.particleFlowClusterECAL.inputECAL.getModuleLabel()=="particleFlowClusterECALWithTimeSelected":
    print "3D Timing: ON 3DTiming"
else:
    print "3D Timing: OFF"
