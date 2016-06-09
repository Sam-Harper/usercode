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
  dEtaInConstTerm(0),
  dEtaInGradTerm(0),
  maxDPhiIn(0),
  maxHadem(0),
  hademConstTerm(0),
  maxSigmaIEtaIEta(0),
  minE2x5Over5x5(0),
  minE1x5Over5x5(0),
  maxNrSatCrysForShowerShapeCut(999),
  //em+had D1 calo isolation
  isolEmHadDepth1ConstTerm(0),
  isolEmHadDepth1GradTerm(0),
  isolEmHadDepth1GradStart(0),
  isolEmHadDepth1RhoEA(0), //80X
  //had D1 isolation
  isolHadDepth2ConstTerm(0),
  isolHadDepth2GradTerm(0),
  isolHadDepth2GradStart(0),
  isolHadDepth2RhoEA(0), //80X
  //track pt isolation
  isolPtTrksConstTerm(0),
  isolPtTrksGradTerm(0),
  isolPtTrksGradStart(0),
  isolPtTrksRhoStart(0),//80X
  isolPtTrksRhoEA(0),//80X
  useValMapForTrkIso(0),
  //WP80
  maxIsolPtTrksRel03(0),
  isolPtTrksRelRhoEA(0),//80X
  maxIsolEmRel03(0),    
  isolEmRelRhoEA(0),//80X
  maxIsolHadRel03(0),
  isolHadRelRhoEA(0),//80X
  maxNrMissHits(0),
  maxDXY(0)
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
  dEtaInConstTerm = iConfig.getParameter<double>("dEtaInConstTerm");
  dEtaInGradTerm = iConfig.getParameter<double>("dEtaInGradTerm");
  maxDPhiIn = iConfig.getParameter<double>("maxDPhiIn");
  maxHadem = iConfig.getParameter<double>("maxHadem");
  hademConstTerm = iConfig.getParameter<double>("hademConstTerm");
  maxSigmaIEtaIEta = iConfig.getParameter<double>("maxSigmaIEtaIEta");
  minE2x5Over5x5 =iConfig.getParameter<double>("minE2x5Over5x5");
  minE1x5Over5x5 =iConfig.getParameter<double>("minE1x5Over5x5");
  maxNrSatCrysForShowerShapeCut = iConfig.getParameter<int>("maxNrSatCrysForShowerShapeCut");
   
  isolEmHadDepth1ConstTerm = iConfig.getParameter<double>("isolEmHadDepth1ConstTerm");
  isolEmHadDepth1GradTerm = iConfig.getParameter<double>("isolEmHadDepth1GradTerm");
  isolEmHadDepth1GradStart = iConfig.getParameter<double>("isolEmHadDepth1GradStart");
  isolEmHadDepth1RhoEA = iConfig.getParameter<double>("isolEmHadDepth1RhoEA");

  isolHadDepth2ConstTerm = iConfig.getParameter<double>("isolHadDepth2ConstTerm");
  isolHadDepth2GradTerm = iConfig.getParameter<double>("isolHadDepth2GradTerm");
  isolHadDepth2GradStart = iConfig.getParameter<double>("isolHadDepth2GradStart");
  isolHadDepth2RhoEA = iConfig.getParameter<double>("isolHadDepth2RhoEA");

  isolPtTrksConstTerm = iConfig.getParameter<double>("isolPtTrksConstTerm");
  isolPtTrksGradTerm = iConfig.getParameter<double>("isolPtTrksGradTerm");
  isolPtTrksGradStart = iConfig.getParameter<double>("isolPtTrksGradStart");
  isolPtTrksRhoStart = iConfig.getParameter<double>("isolPtTrksRhoStart");
  isolPtTrksRhoEA = iConfig.getParameter<double>("isolPtTrksRhoEA");
  useValMapForTrkIso = iConfig.getParameter<bool>("useValMapForTrkIso");

  maxIsolEmRel03     = iConfig.getParameter<double>("maxIsolEmRel03");//WP80
  isolEmRelRhoEA     = iConfig.getParameter<double>("isolEmRelRhoEA");//WP80
  maxIsolHadRel03    = iConfig.getParameter<double>("maxIsolHadRel03");//WP80
  isolHadRelRhoEA    = iConfig.getParameter<double>("isolHadRelRhoEA");//WP80
  maxIsolPtTrksRel03 = iConfig.getParameter<double>("maxIsolPtTrksRel03");//WP80
  isolPtTrksRelRhoEA = iConfig.getParameter<double>("isolPtTrksRelRhoEA");//WP80
  maxNrMissHits = iConfig.getParameter<int>("maxNrMissHits");
  maxDXY = iConfig.getParameter<double>("maxDXY");

  return *this;
}
