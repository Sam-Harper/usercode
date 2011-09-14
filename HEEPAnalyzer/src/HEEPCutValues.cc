#include "SHarper/HEEPAnalyzer/interface/HEEPCutValues.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEleTypeCodes.h"

heep::EleCutValues::EleCutValues():
  cutMask(0x0),
  minEt(0),
  minEta(0),
  maxEta(0),
  //id variables
  maxDEtaIn(0),
  maxDPhiIn(0),
  maxHadem(0),
  maxSigmaIEtaIEta(0),
  minE2x5Over5x5(0),
  minE1x5Over5x5(0),
  //em+had D1 calo isolation
  isolEmHadDepth1ConstTerm(0),
  isolEmHadDepth1GradTerm(0),
  isolEmHadDepth1GradStart(0),
  //had D1 isolation
  isolHadDepth2ConstTerm(0),
  isolHadDepth2GradTerm(0),
  isolHadDepth2GradStart(0),
  //track pt isolation
  isolPtTrksConstTerm(0),
  isolPtTrksGradTerm(0),
  isolPtTrksGradStart(0),
  //WP80
  maxIsolPtTrksRel03(0),
  maxIsolEmRel03(0),    
  maxIsolHadRel03(0)
{

}

heep::EleCutValues::EleCutValues(const edm::ParameterSet& iConfig)
{
  *this=iConfig;
}

heep::EleCutValues& heep::EleCutValues::operator=(const edm::ParameterSet& iConfig)
{
  cutMask = CutCodes::getCode(iConfig.getParameter<std::string>("cuts"));
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
   
  maxIsolEmRel03     = iConfig.getParameter<double>("maxIsolEmRel03");//WP80
  maxIsolHadRel03    = iConfig.getParameter<double>("maxIsolHadRel03");//WP80
  maxIsolPtTrksRel03 = iConfig.getParameter<double>("maxIsolPtTrksRel03");//WP80



  return *this;
}
