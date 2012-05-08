import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *

heepAnalyzerBarePAT = cms.EDAnalyzer("HEEPAnalyzerBarePAT",
                              eleLabel = cms.InputTag("patElectrons"),
                              barrelCuts = cms.PSet(heepBarrelCuts),
                              endcapCuts = cms.PSet(heepEndcapCuts),
                              applyRhoCorrToEleIsol = cms.bool(True), 
                              eleIsolEffectiveAreas = cms.PSet (heepEffectiveAreas),
                              eleRhoCorrLabel = cms.InputTag("kt6PFJetsForIsolation","rho"),
)
