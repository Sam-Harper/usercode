import FWCore.ParameterSet.Config as cms

#this is the master config file to configure the PAT for HEEP isolations and requirements

#limitations:
# currently does not allow heep isolation to be allowed in the cleaning step

# input pat sequences
from PhysicsTools.PatAlgos.patLayer0_cff import *
from PhysicsTools.PatAlgos.patLayer1_cff import  *

#heep isolation modules (replaces PhysicsTools/PatAlgos/python/recoLayer0/electronIsolation_cff.py
from SHarper.HEEPAnalyzer.HEEPIsolations_cff import *

#the standard e/gamma isolation sequence (schedualled for 3_X but works fine in 2_X so lets use it)
from RecoEgamma.EgammaIsolationAlgos.eleIsolationSequence_cff import *

#we want no selection except except dup removal
#we will handle selection later (as all analyses should)
allLayer0Electrons.isolation = cms.PSet()
allLayer0Electrons.removeDuplicates=cms.bool(True) #only eles removed are dups which should be done at RECO level
allLayer0Electrons.saveAll = cms.string("all")
allLayer0Electrons.markItems = cms.bool(True)
allLayer0Electrons.bitsToIgnore = cms.vstring('Isolation/All','Selection/All','Overlap/All')
allLayer0Electrons.electronSource = cms.InputTag('pixelMatchGsfElectrons')
allLayer0Electrons.isolation = cms.PSet() #disable all isolations at pre-selection level
patElectronIds.failSilently = cms.untracked.bool(True) #we handle our own ID using heep::EleSelector
patElectronIds.associations = cms.VInputTag() #we handle our own ID using heep::EleSelector
allLayer1Electrons.addElectronID = cms.bool(False) #we handle our own ID using heep::EleSelector
allLayer1Electrons.electronIDSources = cms.PSet() #we handle our own ID using heep::EleSelector


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
        src = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositTk"),
        # parameters to compute isolation (Egamma POG defaults)
        deltaR = cms.double(0.3),
        vetos = cms.vstring('0.015', # inner radius veto cone
                        'Threshold(1.0)'),       # threshold on individual track pt
        skipDefaultVeto = cms.bool(True), #we skip default veto because we use "vetos" instead (this is legacy feature)
    ),
    ecal = cms.PSet(
        src = cms.InputTag("patLayer0HEEPElectronIsolations","eleIsoDepositEcalFromHits"),
        # parameters to compute isolation (Egamma POG defaults)
        deltaR = cms.double(0.3),
        vetos = cms.vstring('EcalBarrel:0.045',
                            'EcalBarrel:RectangularEtaPhiVeto(-0.02,0.02,-0.5,0.5)',
                              
                            'EcalBarrel:ThresholdFromTransverse(0.08)',
                            'EcalEndcaps:ThresholdFromTransverse(0.3)',
                            'EcalEndcaps:0.070', 
                            'EcalEndcaps:RectangularEtaPhiVeto(-0.02,0.02,-0.5,0.5)'),
        skipDefaultVeto = cms.bool(True)
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


#now define the HEEP pat sequence
#we need to ensure that the e/gamma isolations are run as well as our modules keying them to pat values
heepPATSequence = cms.Sequence(eleIsolationSequence*
                               patLayer0* 
                               patLayer0HEEPIsolSequence* #has to be before layer1
                               patLayer1)


