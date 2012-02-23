import sys

# Import configurations
import FWCore.ParameterSet.Config as cms

outputReco=False

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




process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

process.GlobalTag.globaltag = cms.string('GR_R_42_V21::All')
process.out = cms.OutputModule("PoolOutputModule",
                                      splitLevel = cms.untracked.int32(0),
                                     # outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
                                      #outputCommands = process.RECOEventContent.outputCommands,
                                       outputCommands = cms.untracked.vstring(""),
                                      fileName = cms.untracked.string("dummy.root"),
                                      SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('p')),
                                      dataset = cms.untracked.PSet(
                                          dataTier = cms.untracked.string('HLTDEBUG'),
                                          #      filterName = cms.untracked.string('JETMETFilter'),
                                          
                                          
                                          )
                                      )
process.load("PhysicsTools.PatAlgos.patSequences_cff")


# Get a list of good primary vertices, in 42x, these are DAF vertices
from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
process.goodOfflinePrimaryVertices = cms.EDFilter(
    "PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
    src=cms.InputTag('offlinePrimaryVertices')
    )


# Configure PAT to use PF2PAT instead of AOD sources
# this function will modify the PAT sequences. It is currently 
# not possible to run PF2PAT+PAT and standart PAT at the same time
from PhysicsTools.PatAlgos.tools.pfTools import *
postfix = "PFlow"
usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC=False, postfix=postfix)
process.pfPileUpPFlow.Enable = True
process.pfPileUpPFlow.checkClosestZVertex = cms.bool(False)
process.pfPileUpPFlow.Vertices = cms.InputTag('goodOfflinePrimaryVertices')
process.pfJetsPFlow.doAreaFastjet = True
process.pfJetsPFlow.doRhoFastjet = False


# Compute the mean pt per unit area (rho) from the
# PFchs inputs
from RecoJets.JetProducers.kt4PFJets_cfi import kt4PFJets
process.kt6PFJetsPFlow = kt4PFJets.clone(
    rParam = cms.double(0.6),
    src = cms.InputTag('pfNoElectron'+postfix),
    doAreaFastjet = cms.bool(True),
    doRhoFastjet = cms.bool(True)
    )
process.patJetCorrFactorsPFlow.rho = cms.InputTag("kt6PFJetsPFlow", "rho")

# Add the PV selector and KT6 producer to the sequence
getattr(process,"patPF2PATSequence"+postfix).replace(
    getattr(process,"pfNoElectron"+postfix),
    getattr(process,"pfNoElectron"+postfix)*process.kt6PFJetsPFlow )

process.patseq = cms.Sequence(    
    process.goodOfflinePrimaryVertices*
    getattr(process,"patPF2PATSequence"+postfix)
    )

process.patJetCorrFactors.levels = cms.vstring('L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual')

### Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_BeamGas_HF_Beam1_v2")
#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Mu40_eta2p1_v*")
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_*")
process.skimHLTFilter.throw = False
process.skimHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")

import FWCore.ParameterSet.Config as cms

process.JetFilter = cms.EDFilter("JetFilter",
                                jetTag= cms.InputTag("selectedPatJetsPFlow"),
                                ptCut1=cms.double(-1),
                                ptCut2=cms.double(-1),
                                nrToPassCut1=cms.int32(-1),
                                nrToPassCut2=cms.int32(-1),
                                reverseCuts=cms.bool(False),
                                outputFilename=cms.string("")
                                )


print 

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
process.p = cms.Path(process.patseq*process.JetFilter)

process.load('Configuration/EventContent/EventContent_cff')



    # Adjust the event content
process.out.outputCommands += [
        'keep *_selectedPat*_*_*',
        'keep *_goodOfflinePrimaryVertices*_*_*',    
        'keep double_*PFlow*_*_PAT'
]
if outputReco:
    process.HLTOutput_sam = cms.EndPath(process.out)


#print process.RECOEventContent.outputCommands

    
isCrabJob=False

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    if outputReco:
        process.out.fileName= "OUTPUTFILE"
    else:
        process.JetFilter.outputFilename="OUTPUTFILE"
       
else:
    print "using user specified filename"
    if outputReco:
        process.out.fileName= sys.argv[len(sys.argv)-1]
    else:
        process.JetFilter.outputFilename= sys.argv[len(sys.argv)-1]
      


print process.patJetCorrFactors.levels

##import PhysicsTools.PythonAnalysis.LumiList as LumiList
##import FWCore.ParameterSet.Types as CfgTypes
##myLumis = LumiList.LumiList(filename = 'goodList.json').getCMSSWString().split(',')
##process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
##process.source.lumisToProcess.extend(myLumis)



