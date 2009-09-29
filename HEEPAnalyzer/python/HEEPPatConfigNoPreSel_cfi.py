import FWCore.ParameterSet.Config as cms

#this loads in the HEEPPatConfig and then changes things as necessary

from SHarper.HEEPAnalyzer.HEEPPatConfig_cfi import *

allLayer1Electrons.electronSource = cms.InputTag('gsfElectronsNoCuts')
eleIsoDepositTk.src = cms.InputTag("gsfElectronsNoCuts")
eleIsoDepositHcalFromTowers.src = cms.InputTag("gsfElectronsNoCuts")
eleIsoDepositHcalDepth1FromTowers.src = cms.InputTag("gsfElectronsNoCuts")
eleIsoDepositHcalDepth2FromTowers.src = cms.InputTag("gsfElectronsNoCuts")
eleIsoDepositEcalFromHits.src = cms.InputTag("gsfElectronsNoCuts")
