import sys

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("MCSkim")

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


process.eMuFilter = cms.EDFilter("MCEMuTruthFilter",
                                genParticlesTag= cms.InputTag("genParticles"),
                                nrElesRequired=cms.int32(1),
                                nrMuonsRequired=cms.int32(1)
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


process.skimP = cms.Path(process.eMuFilter)


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
    process.output.fileName= "OUTPUTFILE"
  
else:
    print "using user specified filename"
    process.output.fileName= sys.argv[len(sys.argv)-1]




##import PhysicsTools.PythonAnalysis.LumiList as LumiList
##import FWCore.ParameterSet.Types as CfgTypes
##myLumis = LumiList.LumiList(filename = 'goodList.json').getCMSSWString().split(',')
##process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
##process.source.lumisToProcess.extend(myLumis)



