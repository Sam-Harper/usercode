isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
process = cms.Process("REG2")

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.register('isMC',True,options.multiplicity.singleton,options.varType.bool," whether we are running on MC or not")
options.parseArguments()

print(options.inputFiles)
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles),  
                          )


# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(5000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

#Load geometry
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.autoCond import autoCond
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '130X_mcRun3_2023_realistic_postBPix_v2', '')

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")
process.load("Configuration.StandardSequences.Services_cff")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)


process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(options.outputFile)
)

process.egRegTreeMaker = cms.EDAnalyzer("EGScoutingRegTreeMaker",
                                        verticesTag = cms.InputTag("hltScoutingPrimaryVertexPacker","primaryVtx"),
                                        rhoTag = cms.InputTag("hltScoutingPFPacker","rho"),
                                        #genPartsTag = cms.InputTag("prunedGenParticles"),
                                        genPartsTag = cms.InputTag("genParticles"),
                                        puSumTag = cms.InputTag("addPileupInfo"),
                                        scoutElesTag = cms.InputTag("hltScoutingEgammaPacker") 
                                        )

process.p = cms.Path(process.egRegTreeMaker)
isAOD=False
if isAOD:
    process.load("PhysicsTools.PatAlgos.slimming.slimmedAddPileupInfo_cfi")
    process.p.insert(process.slimmedAddPileupInfo,0)




    process.AODSIMoutput = cms.OutputModule("PoolOutputModule",
                                            compressionAlgorithm = cms.untracked.string('LZMA'),
                                            compressionLevel = cms.untracked.int32(4),
                                            dataset = cms.untracked.PSet(
                                            dataTier = cms.untracked.string('AODSIM'),
                                                filterName = cms.untracked.string('')
                                            ),
                                            eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
                                            fileName = cms.untracked.string(options.outputFile.replace(".root","_EDM.root")),
                                            outputCommands = cms.untracked.vstring('drop *',
                                                                                   "keep *_*_*_HLT",
                                                                                   "drop *_hltTriggerSummaryAOD_*_*",
                                                                                   "drop PileupSummaryInfos_*_*_*",
                                                                                   "keep *_slimmedAddPileupInfo_*_*"
                                        )                                           
    )
    process.out = cms.EndPath(process.AODSIMoutput)

else:
    process.egRegTreeMaker.puSumTag = "slimmedAddPileupInfo"
