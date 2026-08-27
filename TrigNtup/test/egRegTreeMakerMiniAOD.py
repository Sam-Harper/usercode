isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
from Configuration.Eras.Era_Run3_2025_cff import Run3_2025
process = cms.Process("EGREGTREE",Run3_2025)


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
from Configuration.AlCa.GlobalTag import GlobalTag

#gt doesnt really matter much as no reco but nice to get it right
process.GlobalTag = GlobalTag(process.GlobalTag, '142X_mcRun3_2025_realistic_v7', '')

print("*************************************************************************")
print("*******WARNING: miniaod is not sutable for Mustache SC regressions*******")
print("******The SC filled are not mustache nor are they completely filled******")
print("*************************************************************************")


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

process.egRegTreeMaker = cms.EDAnalyzer("EGRegTreeMaker",
                                        verticesTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                        rhoTag = cms.InputTag("fixedGridRhoFastjetAllTmp"),
                                        genPartsTag = cms.InputTag("prunedGenParticles"),
                                        puSumTag = cms.InputTag("slimmedAddPileupInfo"),
                                        scTag = cms.VInputTag("reducedEgamma:reducedSuperClusters"),
                                        scAltTag = cms.VInputTag("reducedEgamma:reducedSuperClusters"),
                                        ecalHitsEBTag = cms.InputTag("reducedEgamma","reducedEBRecHits"),
                                        ecalHitsEETag = cms.InputTag("reducedEgamma","reducedEERecHits"),
                                        elesTag = cms.InputTag("slimmedElectrons"),
                                        phosTag = cms.InputTag("slimmedPhotons"),
                                        elesAltTag = cms.VInputTag(),
                                        phosAltTag = cms.VInputTag()
                                        )
if False:
    process.egRegTreeMaker.verticesTag = cms.InputTag("offlinePrimaryVertices")
    process.egRegTreeMaker.rhoTag = cms.InputTag("fixedGridRhoFastjetAllTmp")
    process.egRegTreeMaker.genPartsTag = cms.InputTag("genParticles")
    process.egRegTreeMaker.elesTag = cms.InputTag("gedGsfElectrons")
    process.egRegTreeMaker.phosTag = cms.InputTag("gedPhotons")
    process.egRegTreeMaker.ecalHitsEBTag = cms.InputTag("reducedEcalRecHitsEB")
    process.egRegTreeMaker.ecalHitsEETag = cms.InputTag("reducedEcalRecHitsEE")

    process.load("SHarper.TrigNtup.rePFSuperCluster_cff")
    process.p = cms.Path(process.rePFSuperClusterThresSeq*process.egRegTreeMaker)

else:
    process.p = cms.Path(process.egRegTreeMaker)

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
                                           "keep *_*_*_HEEP",
                                    )                                           
                                   )
#process.out = cms.EndPath(process.AODSIMoutput)
if isCrabJob:
    process.TFileService.fileName = 'OUTPUTFILE'