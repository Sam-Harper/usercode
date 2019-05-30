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
    process.GlobalTag = GlobalTag(process.GlobalTag, '105X_mc2017_realistic_v5', '')
#    process.GlobalTag = GlobalTag(process.GlobalTag, '103X_mc2017_realistic_v1', '')
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
                                        verticesTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                        rhoTag = cms.InputTag("fixedGridRhoFastjetAll"),
                                        genPartsTag = cms.InputTag("prunedGenParticles"),
                                        puSumTag = cms.InputTag("addPileupInfo"),
  
#                                        scTag = cms.VInputTag("reducedEgamma:reducedSuperClusters",),
                                        scTag = cms.VInputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALBarrel","particleFlowSuperClusterECAL:particleFlowSuperClusterECALEndcapWithPreshower"),
                                        scAltTag = cms.VInputTag(),
                                        ecalHitsEBTag = cms.InputTag("reducedEgamma","reducedEBRecHits"),
                                        ecalHitsEETag = cms.InputTag("reducedEgamma","reducedEERecHits"),
                                        elesTag = cms.InputTag("slimmedElectrons")
                                        )

process.egRegTreeMaker.elesTag = cms.InputTag("gedGsfElectrons")
process.egRegTreeMaker.phosTag = cms.InputTag("gedPhotons")
process.egRegTreeMaker.ecalHitsEBTag = cms.InputTag("reducedEcalRecHitsEB")
process.egRegTreeMaker.ecalHitsEETag = cms.InputTag("reducedEcalRecHitsEE")
process.egRegTreeMaker.verticesTag = cms.InputTag("offlinePrimaryVertices")
process.egRegTreeMaker.rhoTag = cms.InputTag("fixedGridRhoFastjetAllTmp")
process.egRegTreeMaker.genPartsTag = cms.InputTag("genParticles")

process.particleFlowSuperClusterECAL = cms.EDProducer(
    "EGSuperClusCorrector",
    colls = cms.VPSet(
        cms.PSet(
            coll = cms.InputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALBarrel",processName=cms.InputTag.skipCurrentProcess()),
            outputLabel = cms.string("particleFlowSuperClusterECALBarrel")
        ),
        cms.PSet(
            coll = cms.InputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALEndcapWithPreshower",processName=cms.InputTag.skipCurrentProcess()),
            outputLabel = cms.string("particleFlowSuperClusterECALEndcapWithPreshower"),
        ),
    ),
    regressionCfg = cms.PSet(
        isHLT = cms.bool(False),
        applySigmaIetaIphiBug = cms.bool(False),
        ecalRecHitsEB = cms.InputTag("reducedEcalRecHitsEB"),
        ecalRecHitsEE = cms.InputTag("reducedEcalRecHitsEE"),
        regressionKeyEB = cms.string('pfscecal_EBCorrection_offline_v2'),
        regressionKeyEE = cms.string('pfscecal_EECorrection_offline_v2'),
        uncertaintyKeyEB = cms.string('pfscecal_EBUncertainty_offline_v2'),
        uncertaintyKeyEE = cms.string('pfscecal_EEUncertainty_offline_v2'),
        vertexCollection = cms.InputTag("offlinePrimaryVertices")
    )
)



process.p = cms.Path(process.particleFlowSuperClusterECAL*process.egRegTreeMaker)

def readSCRegresFromDBFile(process,filename):
    from CondCore.CondDB.CondDB_cfi import CondDB
   # CondDBReg = CondDB.clone(connect = 'sqlite_file:{}'.format(filename))
    CondDBReg = CondDB.clone(connect = 'frontier://FrontierPrep/CMS_CONDITIONS')
    process.scRegres = cms.ESSource("PoolDBESSource",CondDBReg,
                                     DumpStat=cms.untracked.bool(False),
                                     toGet = cms.VPSet(
cms.PSet(record = cms.string("GBRDWrapperRcd"),
         label = cms.untracked.string("pfscecal_EBCorrection_offline_v2"),
         tag = cms.string("pfscecal_EBCorrection_offline_v2_2017UL")),
cms.PSet(record = cms.string("GBRDWrapperRcd"),
         label = cms.untracked.string("pfscecal_EECorrection_offline_v2"),
         tag = cms.string("pfscecal_EECorrection_offline_v2_2017UL")),
cms.PSet(record = cms.string("GBRDWrapperRcd"),
         label = cms.untracked.string("pfscecal_EBUncertainty_offline_v2"),
         tag = cms.string("pfscecal_EBUncertainty_offline_v2_2017UL")),
cms.PSet(record = cms.string("GBRDWrapperRcd"),
         label = cms.untracked.string("pfscecal_EEUncertainty_offline_v2"),
         tag = cms.string("pfscecal_EEUncertainty_offline_v2_2017UL")),
))
    process.es_prefer_scRegres = cms.ESPrefer("PoolDBESSource","scRegres")
    return process
readSCRegresFromDBFile(process,"scReg_2017_UL_realIC.db")



print process.GlobalTag.globaltag
