import sys

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("RecoSkim")

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

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

#process.load('Configuration.StandardSequences.GeometryDB_cff')
#process.load('Configuration.StandardSequences.MagneticField_38T_cff')
#process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
#process.load('Configuration.StandardSequences.Reconstruction_cff')
#process.load('Configuration.StandardSequences.EndOfProcess_cff')

#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.GlobalTag.globaltag = 'GR_P_V22A::All'

### Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()


#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_BeamGas_HF_Beam1_v2")
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Mu40_eta2p1_v*")
#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_DoubleEle33_CaloIdL_v*")
process.skimHLTFilter.throw = False
process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")

import FWCore.ParameterSet.Config as cms

process.MuFilter = cms.EDFilter("MuonFilter",
                                muonTag= cms.InputTag("muons"),
                                ptCut1=cms.double(40),
                                ptCut2=cms.double(-1),
                                nrToPassCut1=cms.int32(1),
                                nrToPassCut2=cms.int32(-1),
                                reverseCuts=cms.bool(False),
                                outputFilename=cms.string("")
                                )


filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""


if(sys.argv[2].find("/castor/")==0):
    filePrefex="rfio:"
process.source = cms.Source("PoolSource",
                          #fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                           fileNames = cms.untracked.vstring(),
                         #   inputCommands = cms.untracked.vstring("drop *_*_*_RECO","keep *_*BeamSpot*_*_*","keep *_muons_*_*"),
                            eventsToProcess =cms.untracked.VEventRange()
                         
)

for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])


#process.myPref = cms.ESPrefer("PoolDBESSource","GlobalTag")
process.skimP = cms.Path(process.skimHLTFilter*process.MuFilter)
outputReco=False
if outputReco:
    process.load('Configuration/EventContent/EventContent_cff')
    process.output = cms.OutputModule("PoolOutputModule",
                                      splitLevel = cms.untracked.int32(0),
                                      outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
                                      #outputCommands = process.RECOEventContent.outputCommands,
                                      #   outputCommands = cms.untracked.vstring("drop *"),
                                      fileName = cms.untracked.string("dummy.root"),
                                      SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('skimP')),
                                      dataset = cms.untracked.PSet(
                                          dataTier = cms.untracked.string('HLTDEBUG'),
                                          #      filterName = cms.untracked.string('JETMETFilter'),
                                          
                                          
                                          )
                                      )
    process.HLTOutput_sam = cms.EndPath(process.output)


#print process.RECOEventContent.outputCommands

isCrabJob=False

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    if outputReco:
        process.output.fileName= "OUTPUTFILE"
    else:
        process.MuFilter.outputFilename="OUTPUTFILE"
else:
    print "using user specified filename"
    if outputReco:
        process.output.fileName= sys.argv[len(sys.argv)-1]
    else:
        process.MuFilter.outputFilename= sys.argv[len(sys.argv)-1]




##import PhysicsTools.PythonAnalysis.LumiList as LumiList
##import FWCore.ParameterSet.Types as CfgTypes
##myLumis = LumiList.LumiList(filename = 'goodList.json').getCMSSWString().split(',')
##process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
##process.source.lumisToProcess.extend(myLumis)



