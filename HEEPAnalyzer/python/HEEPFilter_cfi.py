import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPEventParameters_cfi import *

heepFilter = cms.EDFilter("HEEPFilter",
    heepEventPara                   
)


