# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("TrigEff")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkSummary = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)


import sys
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


# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Ele32_CaloIdT_CaloIsoT_TrkIdT_TrkIsoT_SC17_v*")
process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


#setting up the producer to make the HEEP ID value map
from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *
process.heepId = cms.EDProducer("HEEPIdValueMapProducer",
                                eleLabel = cms.InputTag("gsfElectrons"),
                                barrelCuts = cms.PSet(heepBarrelCutsV31WithMissHitsCut),
                                endcapCuts = cms.PSet(heepEndcapCutsV31WithMissHitsCut)
                                )

process.heepHLTWP80Eff = cms.EDAnalyzer("HEEPHLTWP80TrigEffAnalyser",           
                                   trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
                                   tagFilterName = cms.string("hltEle32CaloIdTCaloIsoTTrkIdTTrkIsoTEle17TrackIsolFilter"),
                                   probeFilterName = cms.string("hltEle32CaloIdTCaloIsoTTrkIdTTrkIsoTSC17HEDoubleFilter"),
                                   wp80FilterName = cms.string("hltEle27WP80TrackIsoFilter"),
                                  
                                   )



process.p = cms.Path(process.skimHLTFilter*
                     process.heepId*
                     process.heepHLTWP80Eff)

##import PhysicsTools.PythonAnalysis.LumiList as LumiList
##import FWCore.ParameterSet.Types as CfgTypes
##myLumis = LumiList.LumiList(filename = 'your.json').getCMSSWString().split(',')
##process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
##process.source.lumisToProcess.extend(myLumis)
