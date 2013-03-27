isMC=False
pfNoPU=True

patCandID=""
if pfNoPU:
    patCandID="PFlow"

# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = cms.string('GR10_P_V5::All')
from Configuration.AlCa.autoCond import autoCond
#if isMC:
  #  process.GlobalTag.globaltag = autoCond['startup'] 
#else:
   # process.GlobalTag.globaltag = "GR_P_V42_AN2::All"


dataset="DoublePhotonHighPt"
#DATASETSOVERWRITE
datasetVersion=""
#DATASETVERSIONOVERWRITE

if datasetVersion.find("13Jul")!=-1:
    process.GlobalTag.globaltag = "FT_53_V6_AN2::All"
elif datasetVersion.find("Aug06")!=-1:
    process.GlobalTag.globaltag = "FT_53_V6_AN2::All"
elif datasetVersion.find("24Aug")!=-1:
    process.GlobalTag.globaltag = "FT_53_V6_AN2::All"   
else:
    process.GlobalTag.globaltag = "GR_P_V42_AN2::All"

print "Global Tag is ",process.GlobalTag.globaltag 

process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("Configuration.StandardSequences.Services_cff")



import sys


hltName="HLT"
process.load("SHarper.SHNtupliser.shNtupliser_cfi")
process.shNtupliser.datasetCode = 0
process.shNtupliser.sampleWeight = 1
process.shNtupliser.gsfEleTag = "gsfElectrons"
process.shNtupliser.addMet = True
process.shNtupliser.addJets = True
process.shNtupliser.addMuons = True
process.shNtupliser.addCaloTowers = True
process.shNtupliser.addCaloHits = True
process.shNtupliser.addIsolTrks = True
process.shNtupliser.addPFCands = True  
process.shNtupliser.minEtToPromoteSC = 20
process.shNtupliser.fillFromGsfEle = True
process.shNtupliser.minNrSCEtPassEvent = cms.double(-1)
process.shNtupliser.outputGeom = cms.bool(False)
process.shNtupliser.useHLTDebug = cms.bool(False)
process.shNtupliser.hltProcName = hltName
process.shNtupliser.trigResultsTag = cms.InputTag("TriggerResults","",hltName)
process.shNtupliser.trigEventTag = cms.InputTag("hltTriggerSummaryAOD","",hltName)
process.shNtupliser.compTwoMenus = cms.bool(False)
process.shNtupliser.secondHLTTag = cms.string("")
process.shNtupliser.electronTag = cms.untracked.InputTag("patElectrons"+patCandID)
process.shNtupliser.tauTag = cms.untracked.InputTag("patTaus"+patCandID)
process.shNtupliser.muonTag = cms.untracked.InputTag("patMuons"+patCandID)
process.shNtupliser.jetTag = cms.untracked.InputTag("patJets"+patCandID)
process.shNtupliser.photonTag = cms.untracked.InputTag("patPhotons"+patCandID)
process.shNtupliser.metTag = cms.untracked.InputTag("patMETs"+patCandID)
process.shNtupliser.hbheRecHitsTag = cms.InputTag("reducedHcalRecHits","hbhereco")


# Additional output definition
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()


#process.skimHLTFilter.HLTPaths = cms.vstring("HLT_DoublePhoton33*","HLT_Photon*","HLT_DoubleEle33*")
process.skimHLTFilter.HLTPaths = cms.vstring("HLT_DoublePhoton70_v*","HLT_DoublePhoton80_v*", #all the double photons
                                             "HLT_Photon26_Photon18_v*","HLT_Photon36_Photon22_v*", #prescale double photon
                                             "HLT_Photon*CaloIdVL_v*","HLT_Photon135_v*","HLT_Photon150_v*","HLT_Photon160_v*","HLT_Photon*NoHE*", #single photon
                                             "HLT_DoubleEle33_CaloId*")
if dataset=="DoubleElectron":
    print "Double Electron dataset"
    process.skimHLTFilter.HLTPaths = cms.vstring("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_*","HLT_Ele32_CaloIdT_CaloIsoT_TrkIdT_TrkIsoT_SC17_Mass50_v*")
process.skimHLTFilter.throw = False

if dataset=="DoubleElectron":
    process.shNtupliser.addJets = False
    process.shNtupliser.addMuons = False
    process.shNtupliser.addCaloTowers = False
    process.shNtupliser.addCaloHits = False
    process.shNtupliser.addIsolTrks = False
    process.shNtupliser.addPFCands = False


process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)

isCrabJob=False #script seds this if its a crab job

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    #process.shNtupliser.outputFilename= "OUTPUTFILE"
    process.TFileService.fileName= "OUTPUTFILE"
    process.shNtupliser.datasetCode = DATASETCODE
    #process.shNtupliser.sampleWeight = SAMPLEWEIGHT
    process.shNtupliser.sampleWeight = 1
else:
    print "using user specified filename"
    #process.shNtupliser.outputFilename= sys.argv[len(sys.argv)-1]
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]
    process.shNtupliser.datasetCode = 0
    process.shNtupliser.sampleWeight = 1


filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""
if(sys.argv[2].find("/castor/")==0):
    filePrefex="rfio:"
process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])

process.load("Configuration.EventContent.EventContent_cff")
process.out = cms.OutputModule("PoolOutputModule",
    process.FEVTEventContent,
    dataset = cms.untracked.PSet(dataTier = cms.untracked.string('RECO')),
     fileName = cms.untracked.string("ihateyoupatdevelopersIreallydo.root"),
)
#process.out.outputCommands = cms.untracked.vstring('drop *','keep *_MEtoEDMConverter_*_RelValTest')
#
#outPath = cms.EndPath(out)

process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                                      vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                                      minimumNDOF = cms.uint32(4) ,
 						      maxAbsZ = cms.double(24),	
 						      maxd0 = cms.double(2)	
                                                      )

process.load("PhysicsTools.PatAlgos.patSequences_cff")

from PhysicsTools.PatAlgos.tools.coreTools import *

from RecoJets.JetProducers.kt4PFJets_cfi import *
#removeSpecificPATObjects(process, ['Taus'])
if pfNoPU:
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
    print "pf2pat running"
    usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC=False, postfix=postfix,
              jetCorrections=('AK5PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual']),
              pvCollection=cms.InputTag('goodOfflinePrimaryVertices'),
            

              )
    
    process.pfPileUpPFlow.checkClosestZVertex = cms.bool(False)
    process.patseq = cms.Sequence(    
        process.goodOfflinePrimaryVertices*
        getattr(process,"patPF2PATSequence"+postfix)
        )

    removeSpecificPATObjects(process, ['Taus'],postfix=postfix)
#removeMCMatching(process, ['All'])
else:
    removeSpecificPATObjects(process, ['Taus'])
    removeMCMatching(process, ['All'])
    #PAT for some reason wants to re-run this
    process.kt6PFJets = kt4PFJets.clone(
        rParam = cms.double(0.6),
        doAreaFastjet = cms.bool(True),
        doRhoFastjet = cms.bool(True)
    )
    inputJetCorrLabel = ('AK5PF', ['L1Offset', 'L2Relative', 'L3Absolute','L2L3Residual'])
  
    # add pf met
    from PhysicsTools.PatAlgos.tools.metTools import *
    addPfMET(process, 'PF')

    # Add PF jets
    from PhysicsTools.PatAlgos.tools.jetTools import *
    switchJetCollection(process,cms.InputTag('ak5PFJets'),
                        doJTA        = True,
                        doBTagging   = True,
                        jetCorrLabel = inputJetCorrLabel,
                        doType1MET   = True,
                        genJetCollection=cms.InputTag("ak5GenJets"),
                        doJetID      = True,
                        btagdiscriminators=['jetBProbabilityBJetTags','jetProbabilityBJetTags','trackCountingHighPurBJetTags','trackCountingHighEffBJetTags'],
                        )
    process.patJets.addTagInfos = True
    process.patJets.tagInfoSources  = cms.VInputTag(
        cms.InputTag("secondaryVertexTagInfosAOD"),
    )
    
#process.patJetCorrFactors.levels = cms.vstring('L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual')
#process.patJetCorrFactors.levels = cms.vstring('L1Offset', 'L2Relative', 'L3Absolute')



from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFElectronIso
process.eleIsoSequence = setupPFElectronIso(process, 'gsfElectrons')

#for isolation correction

process.kt6PFJetsForIsolation = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True )
process.kt6PFJetsForIsolation.Rho_EtaMax = cms.double(2.5)

process.load('RecoMET.METFilters.ecalLaserCorrFilter_cfi')
process.ecalLaserCorrFilter.taggingMode= True


if  pfNoPU:
   
    process.p = cms.Path(process.skimHLTFilter*#*process.ecalLaserCorrFilter*
        process.primaryVertexFilter*
        process.pfParticleSelectionSequence* process.eleIsoSequence* 
        process.patseq*
        process.kt6PFJetsForIsolation*
        process.shNtupliser)
   
    
else:
    print process.patDefaultSequence
    process.p = cms.Path(process.skimHLTFilter*process.ecalLaserCorrFilter*
        process.primaryVertexFilter*
        process.pfParticleSelectionSequence* process.eleIsoSequence* 
        process.patDefaultSequence*
        process.kt6PFJetsForIsolation*
        process.shNtupliser)


