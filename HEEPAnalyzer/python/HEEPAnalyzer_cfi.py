import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPEventParameters_cfi import *

heepAnalyzer = cms.EDAnalyzer("HEEPAnalyzer",
    heepEventPara                   
)


