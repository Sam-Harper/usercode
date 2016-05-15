isCrabJob=False #script seds this if its a crab job


# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("TrigComp")

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
process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring()
)

if isCrabJob==False:
    from SHarper.SHNtupliser.addInputFiles import addInputFiles
    addInputFiles(process.source,sys.argv[2:len(sys.argv)-1])

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("test.root")
)

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.TFileService.fileName= "OUTPUTFILE"
else:
    print "using user specified filename"
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)



process.scSizeTreeMaker = cms.EDAnalyzer("SCSizeTreeMaker",
                                         l1EGTag = cms.InputTag('hltCaloStage2Digis','EGamma' ),
                                         seededCands=cms.InputTag("hltEgammaCandidates"),
                                         unseededCands=cms.InputTag("hltEgammaCandidatesUnseeded"),
                                         pfClusters=cms.InputTag("hltParticleFlowClusterECALL1Seeded")
)
#load geometry
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond['run2_data']


process.p = cms.Path(process.scSizeTreeMaker)
