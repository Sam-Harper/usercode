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
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

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
    outputCommands = process.RECODEBUGEventContent.outputCommands,
    fileName = cms.untracked.string('TOSED:OUTPUTFILE'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('GEN-SIM-RECO')
    )
)
process.RECOSIMoutput.outputCommands.append('keep *PSimHits_g4SimHits_*_*')

if not isCrabJob:
    process.RECOSIMoutput.fileName = cms.untracked.string(sys.argv[len(sys.argv)-1])

# Additional output definition

# Other statements
process.mix.playback = True
process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)
process.RandomNumberGeneratorService.restoreStateLabel=cms.untracked.string("randomEngineStateProducer")
from Configuration.AlCa.GlobalTag import GlobalTag

if isCrabJob:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'TOSED:GLOBALTAG::All', '')
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, sys.argv[2]+"::All", '')  


from CondCore.DBCommon.CondDBSetup_cfi import *
process.ecalES1 = cms.ESSource("PoolDBESSource",CondDBSetup,
                                        connect = cms.string("frontier://FrontierProd/CMS_COND_31X_ECAL"),
                                        toGet = cms.VPSet(
                                            cms.PSet(record = cms.string("EcalChannelStatusRcd"),tag=cms.string("EcalChannelStatus_coll12_v1_mc")),
                                            cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),tag=cms.string("EcalIntercalibConstants_2011_V3_Bon_start_mc")),
                                            cms.PSet(record = cms.string("EcalLaserAlphasRcd"),tag=cms.string("EcalLaserAlphas_mc")),
                                            cms.PSet(record = cms.string("EcalPedestalsRcd"),tag=cms.string("EcalPedestals_v02_mc")),
                                            cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),tag=cms.string("EcalLaserAPDPNRatios_p1p2p3_v2_mc")),
                                                          )
                                        )

process.ecalES2 = cms.ESSource("PoolDBESSource",CondDBSetup,
                                        connect = cms.string("frontier://FrontierProd/CMS_COND_34X_ECAL"),
                                        toGet = cms.VPSet(
                                          #  cms.PSet(record = cms.string("EcalSRSettingsRcd"),tag=cms.string("null")),
                                            cms.PSet(record = cms.string("EcalTPGLinearizationConstRcd"),tag=cms.string("EcalTPGLinearizationConst_beamv5_startup_mc")),
                                   )
                               )


process.ecalES4 = cms.ESSource("PoolDBESSource",CondDBSetup,
                                        connect = cms.string("frontier://FrontierProd/CMS_COND_ECAL_000"),
                                        toGet = cms.VPSet(
                                             cms.PSet(record = cms.string("EcalLinearCorrectionsRcd"),tag=cms.string("EcalLinearCorrections_mc")),
                                   )
                               )


process.ecalES5 = cms.ESSource("PoolDBESSource",CondDBSetup,
                                        connect = cms.string("frontier://FrontierProd/CMS_COND_31X_PRESHOWER"),
                                        toGet = cms.VPSet(
                                            cms.PSet(record = cms.string("ESChannelStatusRcd"),tag=cms.string("ESChannelStatus_LG_V04_mc")),
                                   )
                               )

process.es_prefer_ecal1 = cms.ESPrefer("PoolDBESSource","ecalES1")
process.es_prefer_ecal2 = cms.ESPrefer("PoolDBESSource","ecalES2")
#process.es_prefer_ecal3 = cms.ESPrefer("PoolDBESSource","ecalES3")
process.es_prefer_ecal4 = cms.ESPrefer("PoolDBESSource","ecalES4")
process.es_prefer_ecal5 = cms.ESPrefer("PoolDBESSource","ecalES5")

process.mcFilter = cms.EDFilter("MCTruthFilter",
                                   genParticlesTag = cms.InputTag("genParticles"),
                                    pid=cms.int32(11)
                                )

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.mcFilter*process.RawToDigi)
process.L1Reco_step = cms.Path(process.mcFilter*process.L1Reco)
process.reconstruction_step = cms.Path(process.mcFilter*process.reconstruction)
process.eventinterpretaion_step = cms.Path(process.mcFilter*process.EIsequence)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)


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
