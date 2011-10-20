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
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_MET120_HBHENoiseFiltered_v6")
process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("SHarper.TrigTools.trigRateNtupMaker_cfi")
process.trigRateNtupMaker.outputFilename= sys.argv[len(sys.argv)-1]
process.trigRateNtupMaker.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT")
#these are the filters which the P4s are going to be saved for
process.trigRateNtupMaker.filterNames = cms.vstring("hltQuadJetCentral",
                                                    "hltL1sL1HTT100",
                                                    "hltPhoton90CaloIdVLIsoLTrackIsoFilter",
                                                    "hltMET80",
                                                    "hltCenJet80MCentralRegional",
                                                    "hltCenJet80CentralRegional",
                                                    "hltMET100",
                                                    "hltPFMHT150Filter",
                                                    "hltQuadJet70",
                                                    "hltSingleMu13L3Filtered13",
                                                    "hltDiMuonL3p5PreFiltered8",
                                                    "hltL1sL1DoubleMu3p5",
                                                    "hltL3fL1sMu16Eta2p1L1f0L2f16QL3Filtered60",
                                                    "hltL3IsoL1sMu14Eta2p1L1f0L2f14QL2IsoL3f24L3IsoFiltered",
                                                    "hltL1sL1DoubleMu0",
                                                    "hltDiMuonL2PreFiltered0",
                                                    "hltL2DoubleMu30NoVertexL2PreFiltered",
                                                    "hltL2DoubleMu23NoVertexL2PreFiltered",
                                                    "hltSingleJet300Regional",
                                                    "hltEle33CaloIdLPixelMatchFilter",
                                                    "hltDiEle33CaloIdLPixelMatchDoubleFilter"
                                                  
                                                )


process.p = cms.Path(#process.skimHLTFilter*                  
                     process.trigRateNtupMaker)

##import PhysicsTools.PythonAnalysis.LumiList as LumiList
##import FWCore.ParameterSet.Types as CfgTypes
##myLumis = LumiList.LumiList(filename = 'goodList.json').getCMSSWString().split(',')
##process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
##process.source.lumisToProcess.extend(myLumis)
