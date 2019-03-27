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
    process.GlobalTag = GlobalTag(process.GlobalTag, '102X_upgrade2018_realistic_v15', '')
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
#                                        scTag = cms.VInputTag("reducedEgamma:reducedSuperClusters",),
                                        scTag = cms.VInputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALBarrel","particleFlowSuperClusterECAL:particleFlowSuperClusterECALEndcapWithPreshower"),
                                        ecalHitsEBTag = cms.InputTag("reducedEgamma","reducedEBRecHits"),
                                        ecalHitsEETag = cms.InputTag("reducedEgamma","reducedEERecHits"),
                                        elesTag = cms.InputTag("slimmedElectrons")
                                        )
process.egRegTreeMaker.scTag = cms.VInputTag("lowPtGsfElectronSuperClusters")
process.egRegTreeMaker.elesTag = cms.InputTag("slimmedLowPtElectrons")
process.egRegTreeMaker.ecalHitsEBTag = cms.InputTag("ecalRecHit","EcalRecHitsEB")
process.egRegTreeMaker.ecalHitsEETag = cms.InputTag("ecalRecHit","EcalRecHitsEE")


process.p = cms.Path( process.egRegTreeMaker)

applyReg = True

if applyReg:
    print "applying regression"
    from RecoEgamma.EgammaTools.regressionModifier_cfi import regressionModifierBParking
    process.slimmedLowPtElectrons = cms.EDProducer(
        "ModifiedElectronProducer",
        src = cms.InputTag("slimmedLowPtElectrons",processName=cms.InputTag.skipCurrentProcess()),
        modifierConfig = cms.PSet( modifications = cms.VPSet(regressionModifierBParking) )
        )

    process.regressionApplication = cms.Sequence(process.slimmedLowPtElectrons)
    process.p.insert(0,process.regressionApplication)


    from CondCore.CondDB.CondDB_cfi import *
    CondDB.connect = 'sqlite_file:bParkingEleReg.db'
    process.lowPtEleRegres = cms.ESSource("PoolDBESSource",CondDB,
                                          DumpStat=cms.untracked.bool(True),
                                          toGet = cms.VPSet(
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string('lowPtElectron_EB_ECALOnly'),
                     tag = cms.string("lowPtElectron_EB_ECALOnly")),                  
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string('lowPtElectron_EB_ECALOnly_sigma'),
                     tag = cms.string("lowPtElectron_EB_ECALOnly_sigma")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string('lowPtElectron_EE_ECALOnly'),
                     tag = cms.string("lowPtElectron_EE_ECALOnly")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string('lowPtElectron_EE_ECALOnly_sigma'),
                     tag = cms.string("lowPtElectron_EE_ECALOnly_sigma")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string("lowPtElectron_EB_ECALTrk"),
                     tag = cms.string("lowPtElectron_EB_ECALTrk")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string("lowPtElectron_EB_ECALTrk_sigma"),
                     tag = cms.string("lowPtElectron_EB_ECALTrk_sigma")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string("lowPtElectron_EE_ECALTrk"),
                     tag = cms.string("lowPtElectron_EE_ECALTrk")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string("lowPtElectron_EE_ECALTrk_sigma"),
                     tag = cms.string("lowPtElectron_EE_ECALTrk_sigma")),
            )
                                          )
    
    process.es_prefer_lowPtEleRegres = cms.ESPrefer("PoolDBESSource","lowPtEleRegres")


print process.GlobalTag.globaltag
