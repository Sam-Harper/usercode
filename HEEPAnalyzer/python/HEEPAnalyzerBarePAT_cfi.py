import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *

heepAnalyzerBarePAT = cms.EDAnalyzer("HEEPAnalyzerBarePAT",
                              eleLabel = cms.InputTag("patElectrons"),
                              barrelCuts = cms.PSet(heepBarrelCuts),
                              endcapCuts = cms.PSet(heepEndcapCuts),
                              eleIsolEffectiveAreas = cms.PSet (
                                         trackerBarrel = cms.double(0.),
                                         trackerEndcap = cms.double(0.),
                                         ecalBarrel = cms.double(0.101),
                                         ecalEndcap = cms.double(0.046),
                                         hcalBarrel = cms.double(0.021),
                                         hcalEndcap = cms.double(0.040)
                                         ),
                              eleRhoCorrLabel = cms.InputTag("kt6PFJets","rho"),    
)
