#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEleTypeCodes.h"

#include "DataFormats/PatCandidates/interface/Electron.h"


void heep::EleSelector::setup(const edm::ParameterSet& iConfig)
{ 
  cutValues_.clear();
  EleCutValues ebCuts(iConfig.getParameter<edm::ParameterSet>("barrelCuts"));
  EleCutValues eeCuts(iConfig.getParameter<edm::ParameterSet>("endcapCuts"));
  addCuts(ebCuts);
  addCuts(eeCuts);
}


int heep::EleSelector::getCutCode(const heep::Ele& ele,int cutMask)const
{
  const EleCutValues* cuts = getCuts(ele.classification());
  if(cuts!=NULL) return getCutCode(ele,*cuts,cutMask);
  else return CutCodes::INVALID;
}

//if it fails a cut, set the bit
//use the enums for speed
int heep::EleSelector::getCutCode(const heep::Ele& ele,const EleCutValues& cuts,int cutMask)
{ 
  int cutCode = 0x0;
  if(ele.et()< cuts.minEt) cutCode |= CutCodes::ET;
  if(ele.detEtaAbs()< cuts.minEta || ele.detEtaAbs()>cuts.maxEta) cutCode |= CutCodes::DETETA;
  if(ele.classification()==40) cutCode |= CutCodes::CRACK;
  if(fabs(ele.dEtaIn()) > cuts.maxDEtaIn ) cutCode |=CutCodes::DETAIN;
  if(fabs(ele.dPhiIn()) > cuts.maxDPhiIn ) cutCode |=CutCodes::DPHIIN;
  if(ele.hOverE()> cuts.maxHadem) cutCode |= CutCodes::HADEM;
  if(ele.scSigmaIEtaIEta()>cuts.maxSigmaIEtaIEta) cutCode |= CutCodes::SIGMAIETAIETA;
  if(ele.isolEmHadDepth1()>( cuts.isolEmHadDepth1ConstTerm + cuts.isolEmHadDepth1GradTerm*(ele.et()<cuts.isolEmHadDepth1GradStart ? 0. : (ele.et()-cuts.isolEmHadDepth1GradStart)))) cutCode |=CutCodes::ISOLEMHADDEPTH1;
  if(ele.scE2x5MaxOver5x5()< cuts.minE2x5Over5x5 && ele.scE1x5Over5x5()<cuts.minE1x5Over5x5) cutCode |=CutCodes::E2X5OVER5X5;
  if(ele.isolHadDepth2()> (cuts.isolHadDepth2ConstTerm + cuts.isolHadDepth2GradTerm*(ele.et()<cuts.isolHadDepth2GradStart ? 0. : (ele.et()-cuts.isolHadDepth2GradStart)))) cutCode |=CutCodes::ISOLHADDEPTH2;
  if(ele.isolPtTrks() > (cuts.isolPtTrksConstTerm + cuts.isolPtTrksGradTerm*(ele.et()<cuts.isolPtTrksGradStart ? 0. : (ele.et()-cuts.isolPtTrksGradStart))))cutCode |=CutCodes::ISOLPTTRKS;
  if(ele.isolNrTrks() > cuts.isolNrTrksConstTerm) cutCode |=CutCodes::ISOLNRTRKS;

  return (cutCode & cuts.cutMask & cutMask) ;
}

int heep::EleSelector::getCutCode(const pat::Electron& ele,int cutMask)const
{
  const EleCutValues* cuts = getCuts(ele.classification());
  if(cuts!=NULL) return getCutCode(ele,*cuts,cutMask);
  else return CutCodes::INVALID;
}

//if it fails a cut, set the bit
//use the enums for speed
int heep::EleSelector::getCutCode(const pat::Electron& ele,const EleCutValues& cuts,int cutMask)
{ 
  int cutCode = 0x0;
  if(ele.et()< cuts.minEt) cutCode |= CutCodes::ET;
  if(fabs(ele.superCluster()->eta())< cuts.minEta || fabs(ele.superCluster()->eta())>cuts.maxEta) cutCode |= CutCodes::DETETA;
  if(ele.classification()==40) cutCode |= CutCodes::CRACK;
  if(fabs(ele.deltaEtaSuperClusterTrackAtVtx()) > cuts.maxDEtaIn ) cutCode |=CutCodes::DETAIN;
  if(fabs(ele.deltaPhiSuperClusterTrackAtVtx()) > cuts.maxDPhiIn ) cutCode |=CutCodes::DPHIIN;
  if(ele.hadronicOverEm()> cuts.maxHadem) cutCode |= CutCodes::HADEM;
  if(ele.scSigmaIEtaIEta()>cuts.maxSigmaIEtaIEta) cutCode |= CutCodes::SIGMAIETAIETA;
  if(ele.ecalIso()+ele.userIso(0)>( cuts.isolEmHadDepth1ConstTerm + cuts.isolEmHadDepth1GradTerm*(ele.et()<cuts.isolEmHadDepth1GradStart ? 0. : (ele.et()-cuts.isolEmHadDepth1GradStart)))) cutCode |=CutCodes::ISOLEMHADDEPTH1; 
  if(ele.scE2x5Max()/ele.scE5x5()< cuts.minE2x5Over5x5 && ele.scE1x5()/ele.scE5x5()<cuts.minE1x5Over5x5) cutCode |=CutCodes::E2X5OVER5X5;
  if(ele.userIso(1)> (cuts.isolHadDepth2ConstTerm + cuts.isolHadDepth2GradTerm*(ele.et()<cuts.isolHadDepth2GradStart ? 0. : (ele.et()-cuts.isolHadDepth2GradStart)))) cutCode |=CutCodes::ISOLHADDEPTH2;
  if(ele.trackIso() > (cuts.isolPtTrksConstTerm + cuts.isolPtTrksGradTerm*(ele.et()<cuts.isolPtTrksGradStart ? 0. : (ele.et()-cuts.isolPtTrksGradStart))))cutCode |=CutCodes::ISOLPTTRKS;
  //no nr trk isolation defined (currently its not defined for heep::Ele either)
  //if(ele.isolNrTrks() > cuts.isolNrTrksConstTerm) cutCode |=CutCodes::ISOLNRTRKS;

  return (cutCode & cuts.cutMask & cutMask) ;
}




const heep::EleCutValues* heep::EleSelector::getCuts(int type)const
{
  int cutIndex=0;
  int nrMatches = 0;
  for(unsigned index=0;index<cutValues_.size();index++){
    if( (EleTypeCodes::makeTypeCode(type) & cutValues_[index].validEleTypes) == EleTypeCodes::makeTypeCode(type)){
      if(nrMatches==0) cutIndex = index;
      nrMatches++;
    }
  }
  if(nrMatches==1) return &cutValues_[cutIndex];
  else return NULL;
}

heep::EleCutValues* heep::EleSelector::getCuts(int type)
{
  const heep::EleSelector* constThis = this;
  const EleCutValues* constCuts = constThis->getCuts(type);
  return const_cast<EleCutValues*>(constCuts); //is this evil code? Possibly, but I think its safe as all I'm doing is basically code reuse by using the const function and then casting the returned value to non const as in this case the object is really non-const 

}
