# Auto generated configuration file
# using: 
# Revision: 1.19 
import FWCore.ParameterSet.Config as cms

process = cms.Process('L1')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.Geometry.GeometryIdeal_cff')

process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(1000),
    limit = cms.untracked.int32(1000)
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
import sys

process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) ) 


# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.19 $'),
    annotation = cms.untracked.string('l1 nevts:1'),
    name = cms.untracked.string('Applications')
)

# Output definition
process.load('Configuration.EventContent.EventContent_cff')
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
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
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


process.mcFilter = cms.EDFilter("MCTruthFilter",
                                genParticlesTag = cms.InputTag("genParticles"),
                                pid=cms.int32(11)
                                )



# Path and EndPath definitions
process.L1simulation_step = cms.Path(
    process.ecalDigis
    +process.hcalDigis
    +process.L1TCaloStage2_PPFromRaw
    +process.shNtupliser
)

if process.shNtupliser.datasetCode==122:
    print "DYToLL MC detected, filtering based on electron decays"
    process.L1simulation_step.insert(0,process.mcFilter)

#process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.L1simulation_step)#,
                         #       process.RECOSIMoutput_step)

