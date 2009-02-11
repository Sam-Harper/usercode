import FWCore.ParameterSet.Config as cms

#limitations:
# currently does not allow heep isolation to be allowed in the cleaning step

# input pat sequences
from PhysicsTools.PatAlgos.patLayer0_cff import *
from PhysicsTools.PatAlgos.patLayer1_cff import  *
#from RecoEgamma.EgammaIsolationAlgos.eleIsoDepositTk_cff import *
#loadin the heep specific isolations, basically hcal depth 1 and depth 2
#from SHarper.HEEPAnalyzer.HEEPIsolationsNoCuts_cff import *
from SHarper.HEEPAnalyzer.HEEPIsolations_cff import *

from RecoEgamma.EgammaIsolationAlgos.eleIsolationSequence_cff import *

#we want no selection except except dup removal
#we will handle selection later (as all analyses should)
allLayer0Electrons.isolation = cms.PSet()
allLayer0Electrons.removeDuplicates=cms.bool(True)
allLayer0Electrons.saveAll = cms.string("all")
allLayer0Electrons.markItems = cms.bool(True)
allLayer0Electrons.bitsToIgnore = cms.vstring('Isolation/All','Selection/All','Overlap/All')
allLayer0Electrons.electronSource = cms.InputTag('pixelMatchGsfElectrons')
allLayer0Electrons.isolation = cms.PSet()

patElectronIds.failSilently = cms.untracked.bool(True)
patElectronIds.associations = cms.VInputTag()

allLayer1Electrons.addElectronID = cms.bool(False)
allLayer1Electrons.electronIDSources = cms.PSet()
#adding in heep isolations to the PAT electron
#this bit is the iso deposits
allLayer1Electrons.isoDeposits = cms.PSet(
        tracker = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositTk"),
        ecal    = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositEcalFromHits"),
        hcal     = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositHcalFromTowers"),
        user    = cms.VInputTag( cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositHcalDepth1FromTowers"), 
                                 cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositHcalDepth2FromTowers"))
)

#now calculate the isolations and store them
allLayer1Electrons.isolation = cms.PSet(
    tracker = cms.PSet(
    # source IsoDeposit
    src = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositTk"),
    # parameters to compute isolation (Egamma POG defaults)
    deltaR = cms.double(0.3),
    vetos = cms.vstring('0.015', # inner radius veto cone
                        'Threshold(1.0)'),       # threshold on individual track pt
    skipDefaultVeto = cms.bool(True),
    ),
    ecal = cms.PSet(
    # source IsoDeposit
    src = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositEcalFromHits"),
    # parameters to compute isolation (Egamma POG defaults)
    deltaR = cms.double(0.3),
    vetos = cms.vstring('EcalBarrel:0.045',
                        'EcalBarrel:RectangularEtaPhiVeto(-0.02,0.02,-0.5,0.5)',

                        'EcalBarrel:ThresholdFromTransverse(0.08)',
                        'EcalEndcaps:ThresholdFromTransverse(0.3)',
                        'EcalEndcaps:0.070', 
                        'EcalEndcaps:RectangularEtaPhiVeto(-0.02,0.02,-0.5,0.5)'), 
    ),
    
    hcal = cms.PSet(
    src = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositHcalFromTowers"),
    # parameters to compute isolation (Egamma POG defaults)
    deltaR = cms.double(0.3),
    vetos = cms.vstring('0.15'),
    skipDefaultVeto = cms.bool(True),
    ),
    user = cms.VPSet(
    cms.PSet(
    src = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositHcalDepth1FromTowers"),
    # parameters to compute isolation (Egamma POG defaults)
    deltaR = cms.double(0.3),
    vetos = cms.vstring('0.15'),
    skipDefaultVeto = cms.bool(True),
    ),

    cms.PSet(
    src = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositHcalDepth2FromTowers"),
    # parameters to compute isolation (Egamma POG defaults)
    deltaR = cms.double(0.3),
    vetos = cms.vstring('0.15'),
    skipDefaultVeto = cms.bool(True),
    )
    
    )#end user VPSet
)


heepPATSequence = cms.Sequence(eleIsolationSequence*
                               patLayer0* 
                               patLayer0HEEPIsolSequence* #has to be before layer1
                               patLayer1)


