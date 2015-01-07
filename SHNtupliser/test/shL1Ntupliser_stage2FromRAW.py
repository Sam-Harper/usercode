# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: l1 -s L1 --pileup=NoPileUp --geometry DB --conditions=auto:startup -n 1 --no_exec --filein=/store/mc/Fall13dr/TT_Tune4C_13TeV-pythia8-tauola/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/007939EF-8075-E311-B675-0025905938AA.root
import FWCore.ParameterSet.Config as cms

process = cms.Process('L1')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(1000),
    limit = cms.untracked.int32(1000)
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
import sys
# Input source
filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""
if(sys.argv[2].find("/castor/")==0):
    filePrefex="rfio:"
process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) ) 


# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.19 $'),
    annotation = cms.untracked.string('l1 nevts:1'),
    name = cms.untracked.string('Applications')
)

# Output definition

process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = cms.untracked.vstring("keep *"),#process.RECOSIMEventContent.outputCommands,
    fileName = cms.untracked.string('L1_stage1.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('')
    )
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:startup', '')
process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_72_V3A::All', '')

# to get input digis
process.load('Configuration.StandardSequences.RawToDigi_cff')

# upgrade calo stage 1
process.load('L1Trigger.L1TCalorimeter.L1TCaloStage2_PPFromRaw_cff')


from SHarper.HEEPAnalyzer.HEEPEventParameters_cfi import *
from SHarper.SHNtupliser.shNtupliserParameters_cfi import *

process.shNtupliser = cms.EDAnalyzer("SHL1Ntupliser", heepEventPara,shNtupPara,
                                     l1CaloClustersTag = cms.InputTag("caloStage2Digis"),
                                     l1CaloTowersTag = cms.InputTag("caloStage2Digis")
   
)
process.shNtupliser.nrGenPartToStore =cms.int32(-1)
process.shNtupliser.outputGeom=cms.bool(False)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","DontActuallyWantThisProduct")
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","","DontActuallyWantThisProduct")
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

isCrabJob=False #script seds this if its a crab job

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.TFileService.fileName= "OUTPUTFILE"
    #process.shNtupliser.outputFilename= "OUTPUTFILE"
    process.shNtupliser.datasetCode = DATASETCODE
    process.shNtupliser.sampleWeight = SAMPLEWEIGHT
else:
    print "using user specified filename"
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]
    #process.shNtupliser.outputFilename= sys.argv[len(sys.argv)-1]
    process.shNtupliser.datasetCode = 101
    process.shNtupliser.sampleWeight = 1

process.stage1L1extraParticles = cms.EDProducer("L1ExtraParticlesProd",
                                          muonSource = cms.InputTag("gcDigis"),
                                          etTotalSource = cms.InputTag("caloStage1LegacyFormatDigis"),
                                          nonIsolatedEmSource = cms.InputTag("caloStage1LegacyFormatDigis","nonIsoEm"),
                                          etMissSource = cms.InputTag("caloStage1LegacyFormatDigis"),
                                          htMissSource = cms.InputTag("caloStage1LegacyFormatDigis"),
                                          produceMuonParticles = cms.bool(True),
                                          forwardJetSource = cms.InputTag("caloStage1LegacyFormatDigis","forJets"),
                                          centralJetSource = cms.InputTag("caloStage1LegacyFormatDigis","cenJets"),
                                          produceCaloParticles = cms.bool(True),
                                          tauJetSource = cms.InputTag("caloStage1LegacyFormatDigis","tauJets"),
                                          isolatedEmSource = cms.InputTag("caloStage1LegacyFormatDigis","isoEm"),
                                          etHadSource = cms.InputTag("caloStage1LegacyFormatDigis"),
                                          hfRingEtSumsSource = cms.InputTag("caloStage1LegacyFormatDigis"),
                                          hfRingBitCountsSource = cms.InputTag("caloStage1LegacyFormatDigis"),
                                          centralBxOnly = cms.bool(True),
                                          ignoreHtMiss = cms.bool(False)
                                          )


process.mcFilter = cms.EDFilter("MCTruthFilter",
                                genParticlesTag = cms.InputTag("genParticles"),
                                pid=cms.int32(11)
                                )



# Path and EndPath definitions
process.L1simulation_step = cms.Path(
    process.ecalDigis
    +process.hcalDigis
    +process.L1TCaloStage2_PPFromRaw
 #   +process.stage1L1extraParticles
    +process.shNtupliser
)

if process.shNtupliser.datasetCode==122:
    print "DYToLL MC detected, filtering based on electron decays"
    process.L1simulation_step.insert(0,process.mcFilter)

#process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.L1simulation_step)#,
                         #       process.RECOSIMoutput_step)

