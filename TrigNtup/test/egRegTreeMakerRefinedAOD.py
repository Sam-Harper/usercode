isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
process = cms.Process("HEEP")

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.register('isMC',True,options.multiplicity.singleton,options.varType.bool," whether we are running on MC or not")
options.parseArguments()

print options.inputFiles
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
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.autoCond import autoCond
from Configuration.AlCa.GlobalTag import GlobalTag
if options.isMC:
#    process.GlobalTag = GlobalTag(process.GlobalTag, '105X_mc2017_realistic_v5', '')
    process.GlobalTag = GlobalTag(process.GlobalTag, '105X_upgrade2018_realistic_v4', '')
else:
    from SHarper.SHNtupliser.globalTags_cfi import getGlobalTagNameData
    globalTagName = getGlobalTagNameData(datasetVersion)
    process.GlobalTag = GlobalTag(process.GlobalTag, globalTagName,'')
    process.GlobalTag = GlobalTag(process.GlobalTag, '103X_dataRun2_v6_AC_v01', '')

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
                                        verticesTag = cms.InputTag("offlinePrimaryVertices"),
                                        rhoTag = cms.InputTag("fixedGridRhoFastjetAllTmp"),
                                        genPartsTag = cms.InputTag("genParticles"),
                                        puSumTag = cms.InputTag("addPileupInfo"),
                                     #   scTag = cms.VInputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALBarrel","particleFlowSuperClusterECAL:particleFlowSuperClusterECALEndcapWithPreshower"),
                                        scTag = cms.VInputTag("particleFlowEGamma",),
                                        scAltTag = cms.VInputTag("particleFlowSuperClusterECALNoThres:particleFlowSuperClusterECALBarrel","particleFlowSuperClusterECALNoThres:particleFlowSuperClusterECALEndcapWithPreshower"),
                                        ecalHitsEBTag = cms.InputTag("reducedEcalRecHitsEB"),
                                        ecalHitsEETag = cms.InputTag("reducedEcalRecHitsEE"),
                                        elesTag = cms.InputTag("gedGsfElectrons"),
                                        phosTag = cms.InputTag("gedPhotons"),
                                        elesAltTag = cms.VInputTag(),
                                        phosAltTag = cms.VInputTag(),
                                        )

process.load("SHarper.TrigNtup.rePFSuperCluster_cff")


process.p = cms.Path(process.rePFSuperClusterThresSeq*process.egRegTreeMaker)
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

def setEventsToProcess(process,eventsToProcess):
    process.source.eventsToProcess = cms.untracked.VEventRange()
    for event in eventsToProcess:
        runnr = event.split(":")[0]
        eventnr = event.split(":")[2]
        process.source.eventsToProcess.append('{runnr}:{eventnr}-{runnr}:{eventnr}'.format(runnr=runnr,eventnr=eventnr))

#eventsToProcess = ['1:1:9322756']
#setEventsToProcess(process,eventsToProcess)

