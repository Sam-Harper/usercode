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
    reportEvery = cms.untracked.int32(100),
    limit = cms.untracked.int32(10000000)
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
process.options = cms.untracked.PSet(

)

process.options = cms.untracked.PSet(

)

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
    fileName = cms.untracked.string('L1.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('')
    )
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'POSTLS170_V3::All', '')

# upgrade calo stage 2
process.load('L1Trigger.L1TCalorimeter.L1TCaloStage2_cff')
#process.load('L1Trigger.L1TCalorimeter.l1tCaloAnalyzer_cfi')



# TTree output file
process.load("CommonTools.UtilAlgos.TFileService_cfi")
process.TFileService.fileName = cms.string('l1t.root')

process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration/StandardSequences/L1HwVal_cff')

# bug fix for missing HCAL TPs in MC RAW
from SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff import HcalTPGCoderULUT
HcalTPGCoderULUT.LUTGenerationMode = cms.bool(True)
process.l1tCaloStage2Layer1Digis.hcalToken = cms.InputTag("valHcalTriggerPrimitiveDigis")


from SHarper.HEEPAnalyzer.HEEPEventParameters_cfi import *
from SHarper.SHNtupliser.shNtupliserParameters_cfi import *

process.shNtupliser = cms.EDAnalyzer("SHL1Ntupliser", heepEventPara,shNtupPara,
                                     l1CaloClustersTag = cms.InputTag("l1tCaloStage2Digis"),
                                     l1CaloTowersTag = cms.InputTag("l1tCaloStage2Digis")
   
)
process.shNtupliser.nrGenPartToStore =cms.int32(-1)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)

process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

#process.load("Configuration.Geometry.GeometryIdeal_cff")
#process.geomPrefer = cms.ESPrefer("ZdcGeometryFromDBEP","")
#process.hcalGeomPrefer = cms.ESPrefer("HcalGeometryFromDBEP","")
#process.trkGeomPerfer = cms.ESPrefer("TrackerGeometricDetESModule","trackerNumberingGeometry")
#process.castorGeomPrefer=cms.ESPrefer("CastorGeometryFromDBEP")
#process.caloTowerGeomPerfer=cms.ESPrefer("CaloTowerGeometryFromDBEP")
#process.ecalBarrelGeomPrefer=cms.ESPrefer("EcalBarrelGeometryFromDBEP")
#process.ecalEndcapGeomPrefer=cms.ESPrefer("EcalEndcapGeometryFromDBEP")
#process.geomPrefer = cms.ESPrefer("PoolDBESSource","ZdcGeometryFromDBEP")
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


# Path and EndPath definitions
process.L1simulation_step = cms.Path(process.ecalDigis
                                     +process.hcalDigis
                                     +process.valHcalTriggerPrimitiveDigis
                                     +process.L1TCaloStage2*process.shNtupliser)
                            #         +process.l1tCaloAnalyzer)
#process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.L1simulation_step)
#                                process.RECOSIMoutput_step)

