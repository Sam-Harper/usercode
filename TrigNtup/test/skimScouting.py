isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
process = cms.Process("SCOUTSkim")

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

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.scoutingToRecoTrackAssociator = cms.EDProducer("EGScoutingRecoTrackAssociator",
    scoutElesTag = cms.InputTag("hltScoutingEgammaPacker"),
    recoElesTag = cms.InputTag("slimmedElectrons"),
)
process.p = cms.Path(process.scoutingToRecoTrackAssociator)

process.scoutOutput = cms.OutputModule("PoolOutputModule",
        compressionAlgorithm = cms.untracked.string('LZMA'),
        compressionLevel = cms.untracked.int32(4),
        dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('AODSIM'),
            filterName = cms.untracked.string('')
        ),
        eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
        fileName = cms.untracked.string(options.outputFile.replace(".root","_EDM.root")),
        outputCommands = cms.untracked.vstring('drop *',
                                                "keep *_*_*_SIM",
                                                "keep *_*_*_HLT",
                                                "drop *_genPUProtons_*_*",
                                                "drop *_hltTriggerSummaryAOD_*_*",
                                                "drop PileupSummaryInfos_*_*_*",
                                                "drop *_hltScoutingPFPacker_*_*",
                                                "keep *_hltScoutingPFPacker_rho_*",
                                                "keep *_slimmedAddPileupInfo_*_*",
                                                "keep *_prunedGenParticles_*_*",
                                                "keep *_packedGenParticles_*_*",
                                                "keep *_scoutingToRecoTrackAssociator_*_*"
    )                                           
)
process.out = cms.EndPath(process.scoutOutput)