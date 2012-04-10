# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("TrigRate")

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
#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Mu15_Photon20_CaloIdL_v14")
#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Mu17_Ele8_CaloIdL_v13")
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")
process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("SHarper.TrigTools.trigRateNtupMaker_cfi")
process.trigRateNtupMaker.outputFilename= sys.argv[len(sys.argv)-1]
process.trigRateNtupMaker.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT3")
#these are the filters which the P4s are going to be saved for
##process.trigRateNtupMaker.filterNames = cms.vstring(
##    "hltEle30CaloIdTTrkIdTPFDiPF25NoPUCleaned",
##    "hltEle27CaloIdTTrkIdTPFDiPF25Cleaned",
##    "hltEle27WP80CentralDiPFJet25CleanedNoPU",
##    "hltEle27WP80CentralDiPFJet25Cleaned",
##    "hltMu15Photon20CaloIdLHEFilter",
##    "hltL1Mu7EG5L3MuFiltered17",
##    "hltL1NonIsoHLTNonIsoMu17Ele8PixelMatchFilter"
##                                                )
process.trigRateNtupMaker.filterNames = cms.vstring(
    "hltPhoton135HEFilter",
    "hltEG200EtFilter",
    "hltEle80CaloIdVTTrkIdTDphiFilter",
    "hltEG70HEFilter",
    "hltDoubleEG70HEDoubleFilter",
    "hltL3fL1sMu16Eta2p1L1f0L2f16QL3Filtered40",
    "hltL3fL1sMu16Eta2p1L1f0L2f16QL3Filtered40Q",
    "hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f24QL3crIsoFiltered10"
    )


process.p = cms.Path(process.skimHLTFilter*                  
                     process.trigRateNtupMaker)

##import PhysicsTools.PythonAnalysis.LumiList as LumiList
##import FWCore.ParameterSet.Types as CfgTypes
##myLumis = LumiList.LumiList(filename = 'goodList.json').getCMSSWString().split(',')
##process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
##process.source.lumisToProcess.extend(myLumis)
