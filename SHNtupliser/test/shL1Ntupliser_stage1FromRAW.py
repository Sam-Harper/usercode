import FWCore.ParameterSet.Config as cms

process = cms.Process('L1')

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryIdeal_cff')


# Select the Message Logger output you would like to see:
process.load('FWCore.MessageService.MessageLogger_cfi')


process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(1000),
    limit = cms.untracked.int32(1000)
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

import sys  
# Input source

process.source = cms.Source("PoolSource",
                            #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                            #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
                            )


process.options = cms.untracked.PSet(

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
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag as customiseGlobalTag
process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'MCRUN2_72_V3A')
#process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'PHYS14_25_V1')
#process.GlobalTag = GlobalTag(process.GlobalTag, globaltag ='MCRUN2_72_V3A')

process.load('L1Trigger.L1TCalorimeter.L1TCaloStage1_PPFromRaw_cff')


# GT
from L1Trigger.Configuration.SimL1Emulator_cff import simGtDigis
process.simGtDigis = simGtDigis.clone()
process.simGtDigis.GmtInputTag = 'simGmtDigis'
process.simGtDigis.GctInputTag = 'caloStage1LegacyFormatDigis'
process.simGtDigis.TechnicalTriggersInputTags = cms.VInputTag( )


from SHarper.HEEPAnalyzer.HEEPEventParameters_cfi import *
from SHarper.SHNtupliser.shNtupliserParameters_cfi import *

process.shNtupliser = cms.EDAnalyzer("SHL1Ntupliser", heepEventPara,shNtupPara,
                                     l1CaloClustersTag = cms.InputTag("caloStage1FinalDigis"),
                                     l1CaloTowersTag = cms.InputTag("caloStage1Digis")
   
)
process.shNtupliser.nrGenPartToStore =cms.int32(-1)
process.shNtupliser.outputGeom=cms.bool(False)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","DontActuallyWantThisProduct")
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","","DontActuallyWantThisProduct")
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)

process.load("Configuration.StandardSequences.MagneticField_cff")
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


    filePrefex="file:"
    if(sys.argv[2].find("/pnfs/")==0):
        filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"
        
    if(sys.argv[2].find("/store/")==0):
        filePrefex=""
    if(sys.argv[2].find("/castor/")==0):
        filePrefex="rfio:"

    for i in range(2,len(sys.argv)-1):
        print filePrefex+sys.argv[i]
        process.source.fileNames.extend([filePrefex+sys.argv[i],])




process.stage1L1extraParticles = cms.EDProducer( "L1ExtraParticlesProd",
    tauJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','tauJets' ),
    etHadSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" ),
    isoTauJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','isoTauJets' ),
    etTotalSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" ),
    centralBxOnly = cms.bool( True ),
    centralJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','cenJets' ),
    etMissSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" ),
    hfRingEtSumsSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" ),
    produceMuonParticles = cms.bool( True ),
    forwardJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','forJets' ),
    ignoreHtMiss = cms.bool( False ),
    htMissSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" ),
    produceCaloParticles = cms.bool( True ),
    muonSource = cms.InputTag( "simGtDigis" ),
    isolatedEmSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','isoEm' ),
    nonIsolatedEmSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','nonIsoEm' ),
    hfRingBitCountsSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
)


process.mcFilter = cms.EDFilter("MCTruthFilter",
                                genParticlesTag = cms.InputTag("genParticles"),
                                pid=cms.int32(11)
                                )


process.L1simulation_step = cms.Path(
    process.L1TCaloStage1_PPFromRaw
    +process.simGtDigis
    #+process.l1ExtraLayer2
    +process.stage1L1extraParticles
    +process.shNtupliser
    )

### Path and EndPath definitions
##process.L1simulation_step = cms.Path(
##    process.ecalDigis
##    +process.hcalDigis
##    +process.L1TCaloStage1_PPFromRaw
##    +process.stage1L1extraParticles
##    +process.shNtupliser
##)

if process.shNtupliser.datasetCode==122:
    print "DYToLL MC detected, filtering based on electron decays"
    process.L1simulation_step.insert(0,process.mcFilter)

#process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

# Schedule definition
#process.schedule = cms.Schedule(process.L1simulation_step)#,
                         #       process.RECOSIMoutput_step)

