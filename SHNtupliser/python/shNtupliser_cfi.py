import FWCore.ParameterSet.Config as cms

from SHarper.HEEPAnalyzer.HEEPEventParameters_cfi import *
from SHarper.SHNtupliser.shNtupliserParameters_cfi import *

shNtupliser = cms.EDAnalyzer("SHNtupliser", heepEventPara,shNtupPara
   
) 


