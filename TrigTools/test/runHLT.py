
def isMCFile(filename):
    if filename.find("GEN-SIM-RAW")!=-1:
        return True
    import subprocess
    output = subprocess.Popen(['edmDumpEventContent',filename],stdout=subprocess.PIPE).communicate()[0]
    if output.find("GenParticle")!=-1:
        return True
    else:
        return False
        

from SHarper.TrigTools.hltCustom import process
#process.load("setup_cff")
import FWCore.ParameterSet.Config as cms
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )


process.hltL1sL1SingleEG10.L1SeedsLogicalExpression = cms.string( "L1_SingleEG20" )
process.hltL1sL1SingleEG35ORDoubleEG1510.L1SeedsLogicalExpression = cms.string( "L1_SingleEG20" )
import sys

isCrabJob=False
if isCrabJob==False:
    filePrefex="file:"
    if(sys.argv[2].find("/pnfs/")==0):
        filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"
        
    if(sys.argv[2].find("/store/")==0):
        filePrefex=""

    if(sys.argv[2].find("/castor/")==0):
        filePrefex="rfio:"
    if(sys.argv[2].find("root:")==0):
        filePrefex=""
    process.source.fileNames = cms.untracked.vstring()
    for i in range(2,len(sys.argv)-1):
        print filePrefex+sys.argv[i]
        process.source.fileNames.extend([filePrefex+sys.argv[i],])

isMC=True
if isCrabJob==False:
    isMC = isMCFile(process.source.fileNames[0])
print "isMC: ",isMC

## process.PrescaleService = cms.Service( "PrescaleService",
##     forceDefault = cms.bool( False ),
##     prescaleTable = cms.VPSet(),
##     lvl1DefaultLabel = cms.string( "7e33" ),
##     lvl1Labels = cms.vstring("7e33","1","2","3","4","5")
## )

process.load('Configuration.EventContent.EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
 #  outputCommands = process.RAWEventContent.outputCommands,
   #outputCommands =  process.FEVTDEBUGHLTEventContent.outputCommands,                       
   outputCommands = process.AODSIMEventContent.outputCommands,
   # outputCommands = cms.untracked.vstring("keep *"),
  fileName = cms.untracked.string(sys.argv[len(sys.argv)-1]),
 # SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('HLT_DoublePho33_v1','HLT_Photon80_v1')),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('HLTDEBUG'),
  #      filterName = cms.untracked.string('JETMETFilter'),
        
                                     
    )
)

process.load('L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi')
process.l1GtTrigReport.L1GtRecordInputTag= cms.InputTag("hltGtDigis")
process.L1AnalyzerEndpath = cms.EndPath( process.l1GtTrigReport )

if isCrabJob:
    process.output.fileName=cms.untracked.string("OUTPUTFILE")

## outputFilenameTemp=sys.argv[len(sys.argv)-1]
## outputFilename= outputFilenameTemp[:outputFilenameTemp.find(".root")] + "_ntup" +outputFilenameTemp[outputFilenameTemp.find(".root"):]
## print outputFilename
## process.TFileService = cms.Service("TFileService",
##                                    fileName = cms.string(outputFilename)
## )

from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
if isMC:
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:run2_mc')
else:
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:run2_hlt_GRun')
    
process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
for pset in process.GlobalTag.toGet.value():
    pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
    # fix for multi-run processing
process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )
process.out = cms.EndPath(process.output)
process.maxEvents.input = cms.untracked.int32( -1 )

process.MessageLogger.suppressWarning = cms.untracked.vstring("hltCtf3HitL1SeededWithMaterialTracksPF","hltL1SeededStartUpElectronPixelSeedsPF","hltCtf3HitActivityWithMaterialTracks","hltActivityStartUpElectronPixelSeeds","hltCtfL1SeededWithMaterialTracksPF","hltL1SeededElectronGsfTracks","hltEcalActivityEgammaRegionalCTFFinalFitWithMaterial","hltActivityElectronGsfTracks","hltEgammaGsfTracksUnseeded","hltEgammaGsfTracks","hltEgammaElectronPixelSeeds","hltEgammaElectronPixelSeedsUnseeded")

if isMC:

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

from SLHCUpgradeSimulations.Configuration.postLS1Customs import *
process = customise_HLT( process )

# remove any instance of the FastTimerService
if 'FastTimerService' in process.__dict__:
    del process.FastTimerService

useFastTimer=False
if useFastTimer:
    # instrument the menu with the FastTimerService
    process.load( "HLTrigger.Timer.FastTimerService_cfi" )
    # this is currently ignored in 7.x, and alway uses the real tim clock
    process.FastTimerService.useRealTimeClock         = True

    # enable specific features
    process.FastTimerService.enableTimingPaths        = True
    process.FastTimerService.enableTimingModules      = True
    process.FastTimerService.enableTimingExclusive    = True

    # print a text summary at the end of the job
    process.FastTimerService.enableTimingSummary      = True
    
    # skip the first path (useful for HLT timing studies to disregard the time spent loading event and conditions data)
    process.FastTimerService.skipFirstPath            = False
    
    # enable per-event DQM plots
    process.FastTimerService.enableDQM                = True

    # enable per-path DQM plots
    process.FastTimerService.enableDQMbyPathActive    = True
    process.FastTimerService.enableDQMbyPathTotal     = True
    process.FastTimerService.enableDQMbyPathOverhead  = True
    process.FastTimerService.enableDQMbyPathDetails   = True
    process.FastTimerService.enableDQMbyPathCounters  = True
    process.FastTimerService.enableDQMbyPathExclusive = True

    # enable per-module DQM plots
    process.FastTimerService.enableDQMbyModule        = True
    process.FastTimerService.enableDQMbyModuleType    = True
    
    # enable per-event DQM sumary plots
    process.FastTimerService.enableDQMSummary         = True

    # enable per-event DQM plots by lumisection
    process.FastTimerService.enableDQMbyLumiSection   = True
    process.FastTimerService.dqmLumiSectionsRange     = 2500    # lumisections (23.31 s)

    # set the time resolution of the DQM plots
    process.FastTimerService.dqmTimeRange             = 1000.   # ms
    process.FastTimerService.dqmTimeResolution        =    5.   # ms
    process.FastTimerService.dqmPathTimeRange         =  100.   # ms
    process.FastTimerService.dqmPathTimeResolution    =    0.5  # ms
    process.FastTimerService.dqmModuleTimeRange       =   40.   # ms
    process.FastTimerService.dqmModuleTimeResolution  =    0.2  # ms

    # set the base DQM folder for the plots
    process.FastTimerService.dqmPath                  = "HLT/TimerService"
    process.FastTimerService.enableDQMbyProcesses     = True
    
    # save the DQM plots in the DQMIO format
    process.dqmOutput = cms.OutputModule("DQMRootOutputModule",
                                         fileName = cms.untracked.string("DQM.root")
                                         )
    process.FastTimerOutput = cms.EndPath( process.dqmOutput )


    
print process.GlobalTag.globaltag
