#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEleTypeCodes.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"


void heep::EleSelector::setup(const edm::ParameterSet& iConfig)
{
  barrelCutValues_ = iConfig.getParameter<edm::ParameterSet>("barrelCuts");
  endcapCutValues_ = iConfig.getParameter<edm::ParameterSet>("endcapCuts");
}

int heep::EleSelector::getCutCode(const heep::Ele& ele,const int cutMask)const
{
  return getCutCode(ele.gsfEle(),ele.gsfEleExtra(),cutMask);
}

int heep::EleSelector::getCutCode(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra,
				  const int cutMask)const
{
  const EleCutValues* cuts = ele.isEB() ? getBarrelCuts() : getEndcapCuts();
  if(cuts!=NULL) return getCutCode(ele,eleExtra,*cuts,cutMask);
  else return CutCodes::INVALID;
}




//if it fails a cut, set the bit    
//use the enums for speed
int heep::EleSelector::getCutCode(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra,const EleCutValues& cuts,const int cutMask)
{ 
  int cutCode = 0x0;
  if(!cuts.passEt(ele,eleExtra)) cutCode |= CutCodes::ET;
  if(!cuts.passDetEta(ele,eleExtra)) cutCode |=CutCodes::DETETA;
  if(!cuts.passCrack(ele,eleExtra)) cutCode |=CutCodes::CRACK;
  if(!cuts.passEcalDriven(ele,eleExtra)) cutCode |= CutCodes::ECALDRIVEN;
  if(!cuts.passDEtaIn(ele,eleExtra)) cutCode |=CutCodes::DETAIN;
  if(!cuts.passDEtaInSeed(ele,eleExtra))   cutCode |=CutCodes::DETAINSEED;
  if(!cuts.passDPhiIn(ele,eleExtra)) cutCode |=CutCodes::DPHIIN;
  if(!cuts.passHadem(ele,eleExtra)) cutCode |= CutCodes::HADEM;
  if(!cuts.passSigmaIEtaIEta(ele,eleExtra)) cutCode |= CutCodes::SIGMAIETAIETA;
  if(!cuts.passIsolEmHadDepth1(ele,eleExtra)) cutCode |=CutCodes::ISOLEMHADDEPTH1; 
  if(!cuts.passE2x5Over5x5(ele,eleExtra)) cutCode |=CutCodes::E2X5OVER5X5;
  if(!cuts.passIsolHadDepth2(ele,eleExtra)) cutCode |=CutCodes::ISOLHADDEPTH2;
  if(!cuts.passIsolPtTrks(ele,eleExtra)) cutCode |=CutCodes::ISOLPTTRKS;
  if(!cuts.passIsolPtTrksRel03(ele,eleExtra)) cutCode |=CutCodes::ISOLPTTRKSREL03; 
  if(!cuts.passIsolEmRel03(ele,eleExtra)) cutCode |=CutCodes::ISOLEMREL03; 
  if(!cuts.passIsolHadRel03(ele,eleExtra)) cutCode |=CutCodes::ISOLHADREL03;   
  if(!cuts.passNrMissHits(ele,eleExtra)) cutCode |=CutCodes::NRMISSHITS; 
  if(!cuts.passDXY(ele,eleExtra)) cutCode |=CutCodes::DXY;

  return (cutCode & cuts.cutMask & cutMask) ;
}



