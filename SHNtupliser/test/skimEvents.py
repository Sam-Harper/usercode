# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("Skim")

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
                        #  fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])


# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(200)
)
##from RecoEgamma.EgammaIsolationAlgos.pfBlockBasedIsolation_cfi import *
##process.particleBasedIsolation = cms.EDProducer("ParticleBasedIsoProducer",
##                                        photonTmpProducer = cms.InputTag("gedPhotonsTmp"),
##                                        photonProducer = cms.InputTag("gedPhotons"),
##                                        electronTmpProducer = cms.InputTag("gedGsfElectronsTmp"),                                                                        
##                                        electronProducer = cms.InputTag("gedGsfElectrons"),                                    
##                                        pfEgammaCandidates = cms.InputTag("particleFlowEGamma"),
##                                        pfCandidates = cms.InputTag("particleFlow"),
##                                        valueMapPhoToEG = cms.string("valMapPFEgammaCandToPhoton"),             
##                                        valueMapPhoPFblockIso = cms.string("gedPhotons"),
##                                        valueMapEleToEG = cms.string(""),
##                                        valueMapElePFblockIso = cms.string("gedGsfElectrons"),
##                                        pfBlockBasedIsolationSetUp=cms.PSet(pfBlockBasedIsolation)                                 
##                                        )
##process.p = cms.Path(process.particleBasedIsolation)
    
process.load('Configuration/EventContent/EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
  outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
#   outputCommands = process.AODSIMEventContent.outputCommands,
   # outputCommands = cms.untracked.vstring("keep *"),
  fileName = cms.untracked.string("dummy.root"),
#  SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('skimP')),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('HLTDEBUG'),
  #      filterName = cms.untracked.string('JETMETFilter'),
        
                                     
    )
)

process.output.outputCommands.extend(cms.untracked.vstring("drop recoGenParticles_*_*_*","drop recoPFJets_*_*_*","drop triggerTriggerEvent_*_*_*","drop recoTrack*_*_*_*","drop recoCaloJets_*_*_*","drop *_generalTracks_*_*","drop recoJet*_*_*_*","drop *FwdPtrs*_*_*_*","drop recoGenJets*_*_*_*","drop ints_genParticles_*_*","drop recoPFTau*_*_*_*","drop recoDeDxData*_*_*_*","drop CastorRecHits*_*_*_*","drop recoCastorTowers_*_*_*","drop HFRecHitsSorted_*_*_*","drop TrackingRecHitsOwned_*_*_*","keep recoTracks_generalTracks_*_*","drop *_*ulti5x5*_*_*","drop recoJPTJets_*_*_*","drop *_*ybridSuperClusters_*_*","keep *_particleFlowClusterECAL_*_*","keep *_particleFlowClusterHCAL_*_*","drop *_muons_*_*","drop *_*onversions_*_*","drop recoRecoChargedRefCandidates_trackRefsForJets_*_*","drop recoPFRecHits_particleFlowRecHitHO_*_*","drop *_offlinePrimaryVerticesWithBS_*_*","keep *_*_particleFlowEGamma_*" ))

isCrabJob=False

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.output.fileName= "TOSED:OUTPUTFILE"
    
else:
    print "using user specified filename"
    process.output.fileName= sys.argv[len(sys.argv)-1]


process.HLTOutput_sam = cms.EndPath(process.output)
