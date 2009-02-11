#include "SHarper/HEEPAnalyzer/interface/HEEPCutValues.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEleTypeCodes.h"

heep::EleCutValues::EleCutValues(const edm::ParameterSet& iConfig)
{
  cutMask = CutCodes::getCode(iConfig.getParameter<std::string>("cuts"));
  validEleTypes = EleTypeCodes::getCode(iConfig.getParameter<std::string>("validEleTypes"));
  minEt = iConfig.getParameter<double>("minEt");
  minEta = iConfig.getParameter<double>("minEta");
  maxEta = iConfig.getParameter<double>("maxEta");

  maxDEtaIn = iConfig.getParameter<double>("maxDEtaIn");
  maxDPhiIn = iConfig.getParameter<double>("maxDPhiIn");
  maxHadem = iConfig.getParameter<double>("maxHadem");
  maxSigmaIEtaIEta = iConfig.getParameter<double>("maxSigmaIEtaIEta");
  minE2x5Over5x5 =iConfig.getParameter<double>("minE2x5Over5x5");
  minE1x5Over5x5 =iConfig.getParameter<double>("minE1x5Over5x5");
 
  isolEmHadDepth1ConstTerm = iConfig.getParameter<double>("isolEmHadDepth1ConstTerm");
  isolEmHadDepth1GradTerm = iConfig.getParameter<double>("isolEmHadDepth1GradTerm");
  isolEmHadDepth1GradStart = iConfig.getParameter<double>("isolEmHadDepth1GradStart");
  
  isolHadDepth2ConstTerm = iConfig.getParameter<double>("isolHadDepth2ConstTerm");
  isolHadDepth2GradTerm = iConfig.getParameter<double>("isolHadDepth2GradTerm");
  isolHadDepth2GradStart = iConfig.getParameter<double>("isolHadDepth2GradStart");

  isolPtTrksConstTerm = iConfig.getParameter<double>("isolPtTrksConstTerm");
  isolPtTrksGradTerm = iConfig.getParameter<double>("isolPtTrksGradTerm");
  isolPtTrksGradStart = iConfig.getParameter<double>("isolPtTrksGradStart");
  
  isolNrTrksConstTerm = iConfig.getParameter<int>("isolNrTrksConstTerm");

}
