# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("TrigExample")

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
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])
if(len(sys.argv)==3):
    print filePrefex+sys.argv[2]
    process.source.fileNames.extend([filePrefex+sys.argv[2],])

# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")
process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("SHarper.TrigTools.trigObjP4AnaExample_cfi")
process.trigObjP4AnaExample.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","","HLT")
process.trigObjP4AnaExample.filterName = cms.string("hltPhoton135HEFilter")
process.trigObjP4AnaExample.pathName = cms.string("HLT_Ele27_CaloIdVT_TrkIdT_CentralJet30_CentralJet25_v1")                                                 

process.p = cms.Path(process.skimHLTFilter*                  
                     process.trigObjP4AnaExample)

##import PhysicsTools.PythonAnalysis.LumiList as LumiList
##import FWCore.ParameterSet.Types as CfgTypes
##myLumis = LumiList.LumiList(filename = 'goodList.json').getCMSSWString().split(',')
##process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
##process.source.lumisToProcess.extend(myLumis)
