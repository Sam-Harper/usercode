#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEleTypeCodes.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"


void heep::EleSelector::setup(const edm::ParameterSet& iConfig)
{
  barrelCutValues_ = iConfig.getParameter<edm::ParameterSet>("barrelCuts");
  endcapCutValues_ = iConfig.getParameter<edm::ParameterSet>("endcapCuts");
  isolEffectAreas_ = iConfig.getParameter<edm::ParameterSet>("eleIsolEffectiveAreas");
}


int heep::EleSelector::getCutCode(const heep::Ele& ele,const int cutMask)const
{
  const EleCutValues* cuts = ele.isEB() ? getBarrelCuts() : getEndcapCuts();
  if(cuts!=NULL) return getCutCode(ele,*cuts,cutMask);
  else return CutCodes::INVALID;
}

//if it fails a cut, set the bit
//use the enums for speed
int heep::EleSelector::getCutCode(const heep::Ele& ele,const EleCutValues& cuts,const int cutMask)
{ 
  int cutCode = 0x0;
  if(ele.et()< cuts.minEt) cutCode |= CutCodes::ET;
  if(ele.detEtaAbs()< cuts.minEta || ele.detEtaAbs()>cuts.maxEta) cutCode |= CutCodes::DETETA;
  if(ele.gsfEle().isEBEtaGap()) cutCode |= CutCodes::CRACK;
  if(!ele.isEcalDriven()) cutCode |= CutCodes::ECALDRIVEN;
  if(fabs(ele.dEtaIn()) > std::max(cuts.dEtaInConstTerm+cuts.dEtaInGradTerm*ele.et(),cuts.maxDEtaIn )) cutCode |=CutCodes::DETAIN;
  if(fabs(ele.dEtaInSeed()) > cuts.maxDEtaIn)   cutCode |=CutCodes::DETAINSEED;
  if(fabs(ele.dPhiIn()) > cuts.maxDPhiIn ) cutCode |=CutCodes::DPHIIN;
  if(ele.hOverE()*ele.superCluster().energy()> cuts.hademConstTerm + cuts.maxHadem*ele.superCluster().energy()) cutCode |= CutCodes::HADEM;
  if(ele.sigmaIEtaIEtaFull5x5()>cuts.maxSigmaIEtaIEta) cutCode |= CutCodes::SIGMAIETAIETA;
  if(ele.isolEmHadDepth1()>( cuts.isolEmHadDepth1ConstTerm + cuts.isolEmHadDepth1GradTerm*(ele.et()<cuts.isolEmHadDepth1GradStart ? 0. : (ele.et()-cuts.isolEmHadDepth1GradStart)))) cutCode |=CutCodes::ISOLEMHADDEPTH1;
  if(ele.e2x5MaxOver5x5Full5x5()< cuts.minE2x5Over5x5 && ele.e1x5Over5x5Full5x5()<cuts.minE1x5Over5x5) cutCode |=CutCodes::E2X5OVER5X5;
  if(ele.isolHadDepth2()> (cuts.isolHadDepth2ConstTerm + cuts.isolHadDepth2GradTerm*(ele.et()<cuts.isolHadDepth2GradStart ? 0. : (ele.et()-cuts.isolHadDepth2GradStart)))) cutCode |=CutCodes::ISOLHADDEPTH2;
  if(ele.isolPtTrks() > (cuts.isolPtTrksConstTerm + cuts.isolPtTrksGradTerm*(ele.et()<cuts.isolPtTrksGradStart ? 0. : (ele.et()-cuts.isolPtTrksGradStart))))cutCode |=CutCodes::ISOLPTTRKS;
  if(ele.isolPtTrksRel03() > cuts.maxIsolPtTrksRel03) cutCode |=CutCodes::ISOLPTTRKSREL03; 
  if(ele.isolEmRel03() > cuts.maxIsolEmRel03 ) cutCode |=CutCodes::ISOLEMREL03; 
  if(ele.isolHadRel03() > cuts.maxIsolHadRel03 ) cutCode |=CutCodes::ISOLHADREL03; 
  if(ele.nrMissHits() > cuts.maxNrMissHits ) cutCode |=CutCodes::NRMISSHITS; 
  if(fabs(ele.dxy()) > cuts.maxDXY) cutCode |=CutCodes::DXY;
  return (cutCode & cuts.cutMask & cutMask) ;
}


int heep::EleSelector::getCutCode(const float rho,const math::XYZPoint &vertex,const reco::GsfElectron& ele,const int cutMask)const
{
  const EleCutValues* cuts = ele.isEB() ? getBarrelCuts() : getEndcapCuts();
  if(cuts!=NULL) return getCutCode(rho,vertex,ele,isolEffectAreas_,*cuts,cutMask);
  else return CutCodes::INVALID;
}

int heep::EleSelector::getCutCode(const float rho,const std::vector<reco::Vertex>& vertices,const reco::GsfElectron& ele,const int cutMask)const
{
  math::XYZPoint vertex(0,0,0);
  if(!vertices.empty()) vertex = vertices[0].position();
  return getCutCode(rho,vertex,ele,cutMask);

}



//if it fails a cut, set the bit    
//use the enums for speed
int heep::EleSelector::getCutCode(const float rho,const math::XYZPoint &vertex,const reco::GsfElectron& ele,const heep::EffectiveAreas& effectAreas,const EleCutValues& cuts,const int cutMask)
{ 
  int cutCode = 0x0;
  //now we need to calculate the et of the gsf electron using supercluster energy
  //as GsfElectron by default uses weighted combination of tracker+supercluster or in certain situations just tracker
  float et = ele.energy()!=0. ? ele.et()/ele.energy()*ele.caloEnergy() : 0.;
  float scEnergy = ele.superCluster()->energy();
  et = ele.et();
  
  if(et< cuts.minEt) cutCode |= CutCodes::ET;
  if(fabs(ele.superCluster()->eta())< cuts.minEta || fabs(ele.superCluster()->eta())>cuts.maxEta) cutCode |= CutCodes::DETETA;
  if(ele.isEBEtaGap()) cutCode |= CutCodes::CRACK;
  if(!ele.ecalDrivenSeed()) cutCode |= CutCodes::ECALDRIVEN;
  if(fabs(ele.deltaEtaSuperClusterTrackAtVtx()) > std::max(cuts.dEtaInConstTerm+cuts.dEtaInGradTerm*ele.et(),cuts.maxDEtaIn )) cutCode |=CutCodes::DETAIN;
  if(fabs(heep::Ele::dEtaInSeed(ele)) > cuts.maxDEtaIn)   cutCode |=CutCodes::DETAINSEED;
  if(fabs(ele.deltaPhiSuperClusterTrackAtVtx()) > cuts.maxDPhiIn ) cutCode |=CutCodes::DPHIIN;
  if(ele.hadronicOverEm()*scEnergy > cuts.hademConstTerm + cuts.maxHadem*scEnergy) cutCode |= CutCodes::HADEM;
  if(ele.scSigmaIEtaIEta()>cuts.maxSigmaIEtaIEta) cutCode |= CutCodes::SIGMAIETAIETA;
  if(ele.dr03EcalRecHitSumEt()+ele.dr03HcalDepth1TowerSumEt() - rho*(effectAreas.ecal(ele.superCluster()->eta())+effectAreas.hcal(ele.superCluster()->eta())) >( cuts.isolEmHadDepth1ConstTerm + cuts.isolEmHadDepth1GradTerm*(et<cuts.isolEmHadDepth1GradStart ? 0. : (et-cuts.isolEmHadDepth1GradStart)))) cutCode |=CutCodes::ISOLEMHADDEPTH1; 
  if(ele.e2x5Max()/ele.e5x5()< cuts.minE2x5Over5x5 && ele.e1x5()/ele.e5x5()<cuts.minE1x5Over5x5) cutCode |=CutCodes::E2X5OVER5X5;
  if(ele.dr03HcalDepth2TowerSumEt() - rho*effectAreas.hcal(ele.superCluster()->eta()) > (cuts.isolHadDepth2ConstTerm + cuts.isolHadDepth2GradTerm*(et<cuts.isolHadDepth2GradStart ? 0. : (et-cuts.isolHadDepth2GradStart)))) cutCode |=CutCodes::ISOLHADDEPTH2;
  if(ele.dr03TkSumPt() - rho*effectAreas.tracker(ele.superCluster()->eta()) > (cuts.isolPtTrksConstTerm + cuts.isolPtTrksGradTerm*(et<cuts.isolPtTrksGradStart ? 0. : (et-cuts.isolPtTrksGradStart))))cutCode |=CutCodes::ISOLPTTRKS;
  if((ele.dr03TkSumPt()- rho*effectAreas.tracker(ele.superCluster()->eta()))/ele.trackMomentumAtVtx().rho() > cuts.maxIsolPtTrksRel03) cutCode |=CutCodes::ISOLPTTRKSREL03; 
  if((ele.dr03EcalRecHitSumEt()- rho*effectAreas.ecal(ele.superCluster()->eta()))/ele.trackMomentumAtVtx().rho() > cuts.maxIsolEmRel03 ) cutCode |=CutCodes::ISOLEMREL03; 
  if((ele.dr03HcalTowerSumEt()- rho*effectAreas.hcal(ele.superCluster()->eta()))/ele.trackMomentumAtVtx().rho()  > cuts.maxIsolHadRel03 ) cutCode |=CutCodes::ISOLHADREL03;   
  if(ele.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS) > cuts.maxNrMissHits ) cutCode |=CutCodes::NRMISSHITS; 
  if(fabs(ele.gsfTrack()->dxy(vertex))>cuts.maxDXY) cutCode |=CutCodes::DXY;

  return (cutCode & cuts.cutMask & cutMask) ;
}



