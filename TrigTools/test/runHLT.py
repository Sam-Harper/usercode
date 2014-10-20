
import FWCore.ParameterSet.Config as cms

def isMCFile(filename):
    import subprocess
    output = subprocess.Popen(['edmDumpEventContent',filename],stdout=subprocess.PIPE).communicate()[0]
    if output.find("GenParticle")!=-1:
        return True
    else:
        return False
        

from SHarper.TrigTools.hlt import process
#process.load("setup_cff")
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

#process.hltL1sL1SingleEG22.L1SeedsLogicalExpression=cms.string("L1_SingleEG20")
process.hltEcalRecHit.cleaningConfig.cThreshold_barrel=cms.double(99999)
process.hltEcalRecHit.cleaningConfig.cThreshold_endcap=cms.double(99999)
process.hltEcalRecHit.cleaningConfig.cThreshold_double=cms.double(99999)

process.HLTriggerFirstPath.insert(1,process.HLTL1GtDigisSequence)

import sys
filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

if(sys.argv[2].find("/castor/")==0):
    filePrefex="rfio:"
if(sys.argv[2].find("root:")==0):
    filePrefex=""
process.source.inputCommands = cms.untracked.vstring("keep *_*_*_*","drop *_hlt*_*_*","keep *_rawDataCollector_*_*")
process.source.fileNames = cms.untracked.vstring()
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])


isMC = isMCFile(process.source.fileNames[0])
print "isMC: ",isMC

process.PrescaleService = cms.Service( "PrescaleService",
    forceDefault = cms.bool( False ),
    prescaleTable = cms.VPSet(),
    lvl1DefaultLabel = cms.string( "7e33" ),
    lvl1Labels = cms.vstring("7e33","1","2","3","4","5")
)

process.load('Configuration.EventContent.EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
 #  outputCommands = process.RAWEventContent.outputCommands,
  # outputCommands =  process.FEVTDEBUGHLTEventContent.outputCommands,                       
    #outputCommands = process.RECOSIMEventContent.outputCommands,
   outputCommands = cms.untracked.vstring("keep *"),
  fileName = cms.untracked.string(sys.argv[len(sys.argv)-1]),
 # SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('HLT_DoublePho33_v1','HLT_Photon80_v1')),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('HLTDEBUG'),
  #      filterName = cms.untracked.string('JETMETFilter'),
        
                                     
    )
)
process.spikeNtup = cms.EDAnalyzer("SpikeNtupleMaker",
                                   recoEcalCands=cms.InputTag("hltEgammaCandidates"),
                                   ecalRecHitEB=cms.InputTag('hltEcalRecHit:EcalRecHitsEB'),
                                   ecalRecHitEE=cms.InputTag('hltEcalRecHit:EcalRecHitsEE'),
                                   genParticles=cms.InputTag('genParticles')
                                   )

#process.ntupPath = cms.Path(process.hltL1sL1SingleEG22*process.spikeNtup)

outputFilenameTemp=sys.argv[len(sys.argv)-1]
outputFilename= outputFilenameTemp[:outputFilenameTemp.find(".root")] + "_ntup" +outputFilenameTemp[outputFilenameTemp.find(".root"):]
print outputFilename
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(outputFilename)
)

from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
if isMC:
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'POSTLS172_V3::All')
else:
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:hltonline')
    
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

process.load('L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi')
process.l1GtTrigReport.L1GtRecordInputTag= cms.InputTag("simGtDigis")
process.L1AnalyzerEndpath = cms.EndPath( process.l1GtTrigReport )

print process.GlobalTag.globaltag


