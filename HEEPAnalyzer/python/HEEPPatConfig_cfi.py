import FWCore.ParameterSet.Config as cms

#this is the master config file to configure the PAT for HEEP isolations and requirements

#limitations:
# currently does not allow heep isolation to be allowed in the cleaning step

# input pat sequences
#from PhysicsTools.PatAlgos.patLayer0_cff import *
#from PhysicsTools.PatAlgos.patLayer1_cff import  *
from PhysicsTools.PatAlgos.patSequences_cff import *
#heep isolation modules (replaces PhysicsTools/PatAlgos/python/recoLayer0/electronIsolation_cff.py
#from SHarper.HEEPAnalyzer.HEEPIsolations_cff import *

#the standard e/gamma isolation sequence (schedualled for 3_X but works fine in 2_X so lets use it)
#from RecoEgamma.EgammaIsolationAlgos.eleIsolationSequence_cff import *

#we want no selection except except dup removal
#we will handle selection later (as all analyses should)
#allLayer0Electrons.isolation = cms.PSet()
#allLayer0Electrons.removeDuplicates=cms.bool(True) #only eles removed are dups which should be done at RECO level
#allLayer0Electrons.saveAll = cms.string("all")
#allLayer0Electrons.markItems = cms.bool(True)
#allLayer0Electrons.bitsToIgnore = cms.vstring('Isolation/All','Selection/All','Overlap/All')
#allLayer0Electrons.electronSource = cms.InputTag('pixelMatchGsfElectrons')
#allLayer0Electrons.isolation = cms.PSet() #disable all isolations at pre-selection level
#patElectronIds.failSilently = cms.untracked.bool(True) #we handle our own ID using heep::EleSelector
#patElectronIds.associations = cms.VInputTag() #we handle our own ID using heep::EleSelector
patElectrons.addElectronID = cms.bool(False) #we handle our own ID using heep::EleSelector
patElectrons.electronIDSources = cms.PSet() #we handle our own ID using heep::EleSelector

#basically we try and stop PAT screwing around with our Jets
#allLayer0Jets.removeOverlaps = cms.PSet()
#allLayer0Jets.saveAll = 'yes'




#now define the HEEP pat sequence
#we need to ensure that the e/gamma isolations are run as well as our modules keying them to pat values
heepPATSequence = cms.Sequence(patCandidates
                               )  
                              
                               


