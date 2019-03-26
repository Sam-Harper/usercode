# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step2 --filein file:EGM-RunIIAutumn18DR-00007_step1.root --fileout file:EGM-RunIIAutumn18DR-00007.root --mc --eventcontent MINIAODSIM --runUnscheduled --datatier MINIAODSIM --conditions 102X_upgrade2018_realistic_v15 --step RAW2DIGI,L1Reco,RECO,RECOSIM,EI,PAT --nThreads 8 --geometry DB:Extended --era Run2_2018,bParking --python_filename recoMiniAODBParkingMC.py --no_exec --customise Configuration/DataProcessing/Utils.addMonitoring -n 122
isCrabJob=False #script seds this if its a crab job
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('RECO',eras.Run2_2018,eras.bParking)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.RecoSim_cff')
process.load('CommonTools.ParticleFlow.EITopPAG_cff')
process.load('PhysicsTools.PatAlgos.slimming.metFilterPaths_cff')
process.load('Configuration.StandardSequences.PATMC_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(5000),
    limit = cms.untracked.int32(10000000)
)
import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.parseArguments()


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step2 nevts:122'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(options.outputFile.replace(".root","_ntup.root"))
)

process.egRegTreeMaker = cms.EDAnalyzer("EGRegTreeMaker",
                                        verticesTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                        rhoTag = cms.InputTag("fixedGridRhoFastjetAll"),
                                        genPartsTag = cms.InputTag("prunedGenParticles"),
#                                        scTag = cms.VInputTag("reducedEgamma:reducedSuperClusters",),
                                        scTag = cms.VInputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALBarrel","particleFlowSuperClusterECAL:particleFlowSuperClusterECALEndcapWithPreshower"),
                                        ecalHitsEBTag = cms.InputTag("reducedEgamma","reducedEBRecHits"),
                                        ecalHitsEETag = cms.InputTag("reducedEgamma","reducedEERecHits"),
                                        elesTag = cms.InputTag("slimmedElectrons")
                                        )
process.egRegTreeMaker.scTag = cms.VInputTag("lowPtGsfElectronSuperClusters")
process.egRegTreeMaker.elesTag = cms.InputTag("slimmedLowPtElectrons")
process.egRegTreeMaker.ecalHitsEBTag = cms.InputTag("ecalRecHit","EcalRecHitsEB")
process.egRegTreeMaker.ecalHitsEETag = cms.InputTag("ecalRecHit","EcalRecHitsEE")


process.MINIAODSIMoutput = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(4),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('MINIAODSIM'),
        filterName = cms.untracked.string('')
    ),
    dropMetaData = cms.untracked.string('ALL'),
    eventAutoFlushCompressedSize = cms.untracked.int32(-900),
    fastCloning = cms.untracked.bool(False),
    fileName = cms.untracked.string(options.outputFile.replace(".root","_EDM.root")), 
    outputCommands = process.MINIAODSIMEventContent.outputCommands,
    overrideBranchesSplitLevel = cms.untracked.VPSet(
        cms.untracked.PSet(
            branch = cms.untracked.string('patPackedCandidates_packedPFCandidates__*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('recoGenParticles_prunedGenParticles__*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('patTriggerObjectStandAlones_slimmedPatTrigger__*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('patPackedGenParticles_packedGenParticles__*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('patJets_slimmedJets__*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('recoVertexs_offlineSlimmedPrimaryVertices__*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('recoCaloClusters_reducedEgamma_reducedESClusters_*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('EcalRecHitsSorted_reducedEgamma_reducedEBRecHits_*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('EcalRecHitsSorted_reducedEgamma_reducedEERecHits_*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('recoGenJets_slimmedGenJets__*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('patJets_slimmedJetsPuppi__*'),
            splitLevel = cms.untracked.int32(99)
        ), 
        cms.untracked.PSet(
            branch = cms.untracked.string('EcalRecHitsSorted_reducedEgamma_reducedESRecHits_*'),
            splitLevel = cms.untracked.int32(99)
        )
    ),
    overrideInputFileSplitLevels = cms.untracked.bool(True),
    splitLevel = cms.untracked.int32(0)
)
process.MINIAODSIMoutput.outputCommands.append("keep *_ecalRecHit_*_*")
#process.MINIAODSIMoutput.outputCommands.append("drop *_packedPFCandidates_*_*")
#process.MINIAODSIMoutput.outputCommands.append("drop *_slimmedJets_*_*")
#process.MINIAODSIMoutput.outputCommands.append("drop *_slimmedMuons_*_*")
#process.MINIAODSIMoutput.outputCommands.append("drop *_slimmedPatTrigger_*_*")
#process.MINIAODSIMoutput.outputCommands.append("drop *_slimmedJetsPuppi_*_*")
#process.MINIAODSIMoutput.outputCommands.append("drop *_slimmedTaus_*_*")
#process.MINIAODSIMoutput.outputCommands.append("drop *_slimmedMETs*_*_*")
#process.MINIAODSIMoutput.outputCommands.append("drop *_slimmedCaloJets_*_*")
#process.MINIAODSIMoutput.outputCommands.append("drop *_slimmedCaloJets_*_*")

process.MINIAODSIMoutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('mcFilter_step'))
# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '102X_upgrade2018_realistic_v15', '')

# Path and EndPath definitions
process.mcFilter = cms.EDFilter("MCPtFilter",
                                genParts = cms.InputTag("genParticles"),
                                pid=cms.int32(11),
                                minPt = cms.double(0.),
                                maxPt = cms.double(30.)
                                )
#process.mcFilterSeq = process.mcFilter

process.mcFilter_step = cms.Path(process.mcFilter*process.egRegTreeMaker)
process.raw2digi_step = cms.Path(process.mcFilter*process.RawToDigi)
#process.L1Reco_step = cms.Path(process.mcFilter*process.L1Reco)
#process.reconstruction_step = cms.Path(process.mcFilter*process.reconstruction)
#process.recosim_step = cms.Path(process.mcFilter*process.recosim)
#process.eventinterpretaion_step = cms.Path(process.mcFilter*process.EIsequence)
process.L1Reco_step = cms.Path(process.mcFilter*process.L1Reco)
process.reconstruction_step = cms.Path(process.mcFilter*process.reconstruction)
process.recosim_step = cms.Path(process.recosim)
process.eventinterpretaion_step = cms.Path(process.mcFilter*process.EIsequence)
#process.Flag_trackingFailureFilter = cms.Path(process.goodVertices+process.trackingFailureFilter)
#process.Flag_goodVertices = cms.Path(process.primaryVertexFilter)
#process.Flag_CSCTightHaloFilter = cms.Path(process.CSCTightHaloFilter)
#process.Flag_trkPOGFilters = cms.Path(process.trkPOGFilters)
#process.Flag_HcalStripHaloFilter = cms.Path(process.HcalStripHaloFilter)
#process.Flag_trkPOG_logErrorTooManyClusters = cms.Path(~process.logErrorTooManyClusters)
#process.Flag_EcalDeadCellTriggerPrimitiveFilter = cms.Path(process.EcalDeadCellTriggerPrimitiveFilter)
#process.Flag_ecalLaserCorrFilter = cms.Path(process.ecalLaserCorrFilter)
#process.Flag_globalSuperTightHalo2016Filter = cms.Path(process.globalSuperTightHalo2016Filter)
#process.Flag_eeBadScFilter = cms.Path(process.eeBadScFilter)
#process.Flag_METFilters = cms.Path(process.metFilters)
#process.Flag_chargedHadronTrackResolutionFilter = cms.Path(process.chargedHadronTrackResolutionFilter)
#process.Flag_globalTightHalo2016Filter = cms.Path(process.globalTightHalo2016Filter)
#process.Flag_CSCTightHaloTrkMuUnvetoFilter = cms.Path(process.CSCTightHaloTrkMuUnvetoFilter)
#process.Flag_HBHENoiseIsoFilter = cms.Path(process.HBHENoiseFilterResultProducer+process.HBHENoiseIsoFilter)
#process.Flag_BadChargedCandidateSummer16Filter = cms.Path(process.BadChargedCandidateSummer16Filter)
#process.Flag_hcalLaserEventFilter = cms.Path(process.hcalLaserEventFilter)
#process.Flag_BadPFMuonFilter = cms.Path(process.BadPFMuonFilter)
#process.Flag_ecalBadCalibFilter = cms.Path(process.ecalBadCalibFilter)
#process.Flag_HBHENoiseFilter = cms.Path(process.HBHENoiseFilterResultProducer+process.HBHENoiseFilter)
#process.Flag_trkPOG_toomanystripclus53X = cms.Path(~process.toomanystripclus53X)
#process.Flag_EcalDeadCellBoundaryEnergyFilter = cms.Path(process.EcalDeadCellBoundaryEnergyFilter)
#process.Flag_BadChargedCandidateFilter = cms.Path(process.BadChargedCandidateFilter)
#process.Flag_trkPOG_manystripclus53X = cms.Path(~process.manystripclus53X)
#process.Flag_BadPFMuonSummer16Filter = cms.Path(process.BadPFMuonSummer16Filter)
#process.Flag_muonBadTrackFilter = cms.Path(process.muonBadTrackFilter)
#process.Flag_CSCTightHalo2015Filter = cms.Path(process.CSCTightHalo2015Filter)
#process.endjob_step = cms.EndPath(process.endOfProcess)
process.MINIAODSIMoutput_step = cms.EndPath(process.MINIAODSIMoutput)

print process.endOfProcess.dumpPython()

# Schedule definition
#process.schedule = cms.Schedule(process.mcFilter_step,process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.recosim_step,process.eventinterpretaion_step,process.Flag_HBHENoiseFilter,process.Flag_HBHENoiseIsoFilter,process.Flag_CSCTightHaloFilter,process.Flag_CSCTightHaloTrkMuUnvetoFilter,process.Flag_CSCTightHalo2015Filter,process.Flag_globalTightHalo2016Filter,process.Flag_globalSuperTightHalo2016Filter,process.Flag_HcalStripHaloFilter,process.Flag_hcalLaserEventFilter,process.Flag_EcalDeadCellTriggerPrimitiveFilter,process.Flag_EcalDeadCellBoundaryEnergyFilter,process.Flag_ecalBadCalibFilter,process.Flag_goodVertices,process.Flag_eeBadScFilter,process.Flag_ecalLaserCorrFilter,process.Flag_trkPOGFilters,process.Flag_chargedHadronTrackResolutionFilter,process.Flag_muonBadTrackFilter,process.Flag_BadChargedCandidateFilter,process.Flag_BadPFMuonFilter,process.Flag_BadChargedCandidateSummer16Filter,process.Flag_BadPFMuonSummer16Filter,process.Flag_trkPOG_manystripclus53X,process.Flag_trkPOG_toomanystripclus53X,process.Flag_trkPOG_logErrorTooManyClusters,process.Flag_METFilters,process.endjob_step)#,process.MINIAODSIMoutput_step)
process.schedule = cms.Schedule(process.mcFilter_step,process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.recosim_step,process.eventinterpretaion_step,process.MINIAODSIMoutput_step)#,process.endjob_step)#,process.MINIAODSIMoutput_step)

process.schedule.associate(process.patTask)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(1)
process.options.numberOfStreams=cms.untracked.uint32(1)

# customisation of the process.

# Automatic addition of the customisation function from Configuration.DataProcessing.Utils
#from Configuration.DataProcessing.Utils import addMonitoring 

#call to customisation function addMonitoring imported from Configuration.DataProcessing.Utils
#process = addMonitoring(process)

# End of customisation functions
#do not add changes to your config after this point (unless you know what you are doing)
from FWCore.ParameterSet.Utilities import convertToUnscheduled
process=convertToUnscheduled(process)

# customisation of the process.

# Automatic addition of the customisation function from PhysicsTools.PatAlgos.slimming.miniAOD_tools
from PhysicsTools.PatAlgos.slimming.miniAOD_tools import miniAOD_customizeAllMC 

#call to customisation function miniAOD_customizeAllMC imported from PhysicsTools.PatAlgos.slimming.miniAOD_tools
process = miniAOD_customizeAllMC(process)

# End of customisation functions

# Customisation from command line

#Have logErrorHarvester wait for the same EDProducers to finish as those providing data for the OutputModule
from FWCore.Modules.logErrorHarvester_cff import customiseLogErrorHarvesterUsingOutputCommands
process = customiseLogErrorHarvesterUsingOutputCommands(process)

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion


if isCrabJob:
    process.MINIAODSIMoutput.fileName = 'OUTPUTFILE'.replace('.root','_MINIAODSIM.root')
    process.TFileService.fileName = 'OUTPUTFILE'.replace('.root','_ntup.root')
