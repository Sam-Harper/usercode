
def isMCFile(filename):
    if filename.find("GEN-SIM-RAW")!=-1:
        return True
    import subprocess
    output = subprocess.Popen(['edmDumpEventContent',filename],stdout=subprocess.PIPE).communicate()[0]
    if output.find("GenParticle")!=-1:
        return True
    else:
        return False
        
import sys
if len(sys.argv)<4:
    print "insufficient parameters, syntax cmsRun runHLT.py inputFile.root outputFile.root"
    exit()

    
from SHarper.TrigTools.hlt import process
#process.load("setup_cff")
import FWCore.ParameterSet.Config as cms
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.hltEcalRecHit.cleaningConfig.cThreshold_barrel=cms.double(99999)
process.hltEcalRecHit.cleaningConfig.cThreshold_endcap=cms.double(99999)
process.hltEcalRecHit.cleaningConfig.cThreshold_double=cms.double(99999)
#process.hltL1sL1SingleEG10.L1SeedsLogicalExpression = cms.string( "L1_SingleEG20" )
#process.hltL1sL1SingleEG35ORDoubleEG1510.L1SeedsLogicalExpression = cms.string( "L1_SingleEG20" )




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

process.spikeNtup = cms.EDAnalyzer("SpikeNtupleMaker",
                                   recoEcalCands=cms.InputTag("hltEgammaCandidatesUnseeded"),
                                   recoEcalCandsL1Seeded=cms.InputTag("hltEgammaCandidates"),
                                   ecalRecHitEB=cms.InputTag('hltEcalRecHit:EcalRecHitsEB'),
                                   ecalRecHitEE=cms.InputTag('hltEcalRecHit:EcalRecHitsEE'),
                                   genParticles=cms.InputTag('genParticles')
                                   )

process.ntupPath = cms.Path( process.HLTDoFullUnpackingEgammaEcalSequence*process.HLTPFClusteringForEgamma*process.hltEgammaCandidates*process.HLTPFClusteringForEgammaUnseeded*process.hltEgammaCandidatesUnseeded*process.spikeNtup)

outputFilenameTemp=sys.argv[len(sys.argv)-1]
outputFilename= outputFilenameTemp[:outputFilenameTemp.find(".root")] + "_ntup" +outputFilenameTemp[outputFilenameTemp.find(".root"):]
print outputFilename
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(outputFilename)
)
process.load('L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi')
process.l1GtTrigReport.L1GtRecordInputTag= cms.InputTag("simGtDigis")
process.L1AnalyzerEndpath = cms.EndPath( process.l1GtTrigReport )

if isCrabJob:
    process.output.fileName=cms.untracked.string("OUTPUTFILE")

## outputFilenameTemp=sys.argv[len(sys.argv)-1]
## outputFilename= outputFilenameTemp[:outputFilenameTemp.find(".root")] + "_ntup" +outputFilenameTemp[outputFilenameTemp.find(".root"):]
## print outputFilename
## process.TFileService = cms.Service("TFileService",
##                                    fileName = cms.string(outputFilename)
## )

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag as customiseGlobalTag
if isMC:
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'MCRUN2_72_V1A')
else:
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:run2_hlt_GRun')
    
process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_CONDITIONS'
#process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
for pset in process.GlobalTag.toGet.value():
    pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
    # fix for multi-run processing
process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )
process.out = cms.EndPath(process.output)
process.maxEvents.input = cms.untracked.int32(-1 )

process.MessageLogger.suppressWarning = cms.untracked.vstring("hltCtf3HitL1SeededWithMaterialTracksPF","hltL1SeededStartUpElectronPixelSeedsPF","hltCtf3HitActivityWithMaterialTracks","hltActivityStartUpElectronPixelSeeds","hltCtfL1SeededWithMaterialTracksPF","hltL1SeededElectronGsfTracks","hltEcalActivityEgammaRegionalCTFFinalFitWithMaterial","hltActivityElectronGsfTracks","hltEgammaGsfTracksUnseeded","hltEgammaGsfTracks","hltEgammaElectronPixelSeeds","hltEgammaElectronPixelSeedsUnseeded")


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
