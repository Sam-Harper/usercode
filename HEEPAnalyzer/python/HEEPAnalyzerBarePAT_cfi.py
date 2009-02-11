import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *

heepAnalyzerBarePAT = cms.EDAnalyzer("HEEPAnalyzerBarePAT",
                              eleLabel = cms.InputTag("allLayer1Electrons"),
                              barrelCuts = cms.PSet(heepBarrelCuts),
                              endcapCuts = cms.PSet(heepEndcapCuts)
)
