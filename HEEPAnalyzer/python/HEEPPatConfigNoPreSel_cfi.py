import FWCore.ParameterSet.Config as cms

#this loads in the HEEPPatConfig and then changes things as necessary
from RecoEgamma.EgammaIsolationAlgos.eleIsoDeposits_cff import eleIsoDepositHcalDepth1FromTowers, eleIsoDepositHcalDepth2FromTowers
from RecoEgamma.EgammaIsolationAlgos.eleIsoFromDeposits_cff import  eleIsoFromDepsHcalDepth1FromTowers, eleIsoFromDepsHcalDepth2FromTowers



from SHarper.HEEPAnalyzer.HEEPPatConfig_cfi import *

patElectrons.electronSource = cms.InputTag('gsfElectronsNoCuts')
eleIsoDepositTk.src = cms.InputTag("gsfElectronsNoCuts")
eleIsoDepositHcalFromTowers.src = cms.InputTag("gsfElectronsNoCuts")
eleIsoDepositHcalDepth1FromTowers.src = cms.InputTag("gsfElectronsNoCuts")
eleIsoDepositHcalDepth2FromTowers.src = cms.InputTag("gsfElectronsNoCuts")
eleIsoDepositEcalFromHits.src = cms.InputTag("gsfElectronsNoCuts")

