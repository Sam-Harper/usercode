import FWCore.ParameterSet.Config as cms

#this loads in the HEEPPatConfig and then changes things as necessary

from SHarper.HEEPAnalyzer.HEEPPatConfig_cfi import *

allLayer0Electrons.electronSource = cms.InputTag('pixelMatchGsfElectronsNoCuts')
eleIsoDepositTk.src = cms.InputTag("pixelMatchGsfElectronsNoCuts")
eleIsoDepositHcalFromTowers.src = cms.InputTag("pixelMatchGsfElectronsNoCuts")
eleIsoDepositHcalDepth1FromTowers.src = cms.InputTag("pixelMatchGsfElectronsNoCuts")
eleIsoDepositHcalDepth2FromTowers.src = cms.InputTag("pixelMatchGsfElectronsNoCuts")
eleIsoDepositEcalFromHits.src = cms.InputTag("pixelMatchGsfElectronsNoCuts")

patHEEPElectronIsolationMaps.collection = cms.InputTag("pixelMatchGsfElectronsNoCuts")
