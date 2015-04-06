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
    input = cms.untracked.int32(10)
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
    compressionLevel = cms.untracked.int32(4),
    compressionAlgorithm = cms.untracked.string('LZMA'),
    eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
    outputCommands = process.AODSIMEventContent.outputCommands,
    fileName = cms.untracked.string('TOSED:OUTPUTFILE'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('AODSIM')
    )
)

if not isCrabJob:
    process.AODSIMoutput.fileName = cms.untracked.string(sys.argv[len(sys.argv)-1])
    
# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag

if isCrabJob:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'TOSED:GLOBALTAG', '')
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, sys.argv[2], '')  


#multi fit needs to know the bunch spacing
#however the pileup info producer does not produce this in 72X
#so if we are running 73X on 72X, we need to fake it
process.addPileupInfo = cms.EDProducer("BXRecordProducer",
                                       bunchSpacing=cms.int32(25) #this should probably be configable via script
                                       )

# Path and EndPath definitions
process.puInfo_step = cms.Path(process.addPileupInfo)
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
process.schedule = cms.Schedule(process.puInfo_step,process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.eventinterpretaion_step,process.endjob_step,process.AODSIMoutput_step)

# customisation of the process.

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.postLS1Customs
from SLHCUpgradeSimulations.Configuration.postLS1Customs import customisePostLS1 

#call to customisation function customisePostLS1 imported from SLHCUpgradeSimulations.Configuration.postLS1Customs
process = customisePostLS1(process)

# End of customisation functions

##from CondCore.DBCommon.CondDBSetup_cfi import *
##process.ecalES1 = cms.ESSource("PoolDBESSource",CondDBSetup,
##                                        connect = cms.string("frontier://FrontierProd/CMS_COND_31X_ECAL"),
##                                        toGet = cms.VPSet(
##                                            cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),tag=cms.string("EcalIntercalib_2011_V3_Bon_mc_subModCor_smeared_1fb")),
                                           
##                                            cms.PSet(record = cms.string("EcalPedestalsRcd"),tag=cms.string("EcalPedestals_208944_200_mc")),
##                                                          )
##                                        )

##process.es_prefer_ecal1 = cms.ESPrefer("PoolDBESSource","ecalES1")
