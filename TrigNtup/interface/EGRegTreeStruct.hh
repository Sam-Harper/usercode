#ifndef SHarper_TrigNtup_SCRegTreeStruct_hh
#define SHarper_TrigNtup_SCRegTreeStruct_hh

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/isFinite.h"

#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"

#include "TTree.h"

namespace reco{
  class SuperCluster;
  class GenParticle;
}

namespace edm{
  class Event;
}
class CaloTopology;

struct ClustStruct { 
  float clusterRawEnergy,clusterDEtaToSeed,clusterDPhiToSeed;
  static std::string contents(){return "clusterRawEnergy/F:clusterDEtaToSeed:clusterDPhiToSeed";}
  void clear(){clusterRawEnergy=clusterDEtaToSeed=clusterDPhiToSeed=0.;}
  void fill(float iClusterRawEnergy,float iClusterDEtaToSeed,float iClusterDPhiToSeed){
    clusterRawEnergy = iClusterRawEnergy;
    clusterDEtaToSeed = iClusterDEtaToSeed;
    clusterDPhiToSeed = iClusterDPhiToSeed;
  }
};


struct EleStruct {
  float et,energy,energyErr,ecalEnergy,ecalEnergyErr,eta,phi,trkEtaMode,trkPhiMode,trkPMode,trkPModeErr,fbrem,corrMean,corrSigma,hademTow,hademCone,trkPInn,trkPtInn,trkPVtx,trkPOut,trkChi2,trkNDof,ecalDrivenSeed,nrSatCrys,scRawEnergy,scRawESEnergy;
  static std::string contents(){return "et/F:energy:energyErr:ecalEnergy:ecalEnergyErr:eta:phi:trkEtaMode:trkPhiMode:trkPMode:trkPModeErr:fbrem:corrMean:corrSigma:hademTow:hademCone:trkPInn:trkPtInn:trkPVtx:trkPOut:trkChi2:trkNDof:ecalDrivenSeed:nrSatCrys:scRawEnergy:scRawESEnergy";}
  void clear(){et=energy=energyErr=ecalEnergy=ecalEnergyErr=eta=phi=trkEtaMode=trkPhiMode=trkPMode=trkPModeErr=fbrem=corrMean=corrSigma=hademTow=hademCone=trkPInn=trkPtInn=trkPVtx=trkPOut=trkChi2=trkNDof=ecalDrivenSeed=nrSatCrys=scRawEnergy=scRawESEnergy=0.;}
  void fill(const reco::GsfElectron& ele);
};

struct PhoStruct {
  float et,energy,energyErr,eta,phi,corrMean,corrSigma,hademTow,hademCone,nrSatCrys,scRawEnergy,scRawESEnergy;
  static std::string contents(){return "et/F:energy:energyErr:eta:phi:corrMean:corrSigma:hademTow:hademCone:nrSatCrys:scRawEnergy:scRawESEnergy";}
  void clear(){et=energy=energyErr=eta=phi=corrMean=corrSigma=hademTow=hademCone=nrSatCrys=scRawEnergy=scRawESEnergy=0.;}
  void fill(const reco::Photon& pho);
};

struct SuperClustStruct {
  float rawEnergy,rawESEnergy,etaWidth,phiWidth,seedClusEnergy,numberOfClusters,numberOfSubClusters,clusterMaxDR,clusterMaxDRDPhi,clusterMaxDRDEta,clusterMaxDRRawEnergy,corrEnergy,scEta,scPhi,seedEta,seedPhi,dEtaSeedSC,dPhiSeedSC,isEB,iEtaOrX,iPhiOrY,iEtaMod5,iPhiMod2,iEtaMod20,iPhiMod20,etaGapCode,phiGapCode,nearbyChanStatus,corrEnergyAlt,rawEnergyAlt,nrClusAlt,scSinTheta,seedSinTheta;
  static std::string contents(){return "rawEnergy/F:rawESEnergy:etaWidth:phiWidth:seedClusEnergy:numberOfClusters:numberOfSubClusters:clusterMaxDR:clusterMaxDRDPhi:clusterMaxDRDEta:clusterMaxDRRawEnergy:corrEnergy:scEta:scPhi:seedEta:seedPhi:dEtaSeedSC:dPhiSeedSC:isEB:iEtaOrX:iPhiOrY:iEtaMod5:iPhiMod2:iEtaMod20:iPhiMod20:etaGapCode:phiGapCode:nearbyChanStatus:corrEnergyAlt:rawEnergyAlt:nrClusAlt:scSinTheta:seedSinTheta";}
  void clear(){
    rawEnergy=rawESEnergy=etaWidth=phiWidth=seedClusEnergy=numberOfClusters=numberOfSubClusters=clusterMaxDR=clusterMaxDRDPhi=clusterMaxDRDEta=clusterMaxDRRawEnergy=corrEnergy=scEta=scPhi=seedEta=seedPhi=dEtaSeedSC=dPhiSeedSC=isEB=iEtaOrX=iPhiOrY=iEtaMod5=iPhiMod2=iEtaMod20=iPhiMod20=etaGapCode=phiGapCode=nearbyChanStatus=corrEnergyAlt=rawEnergyAlt=nrClusAlt=scSinTheta=seedSinTheta=0.;
  }

  void fill(const reco::SuperCluster& sc,const EcalChannelStatus& ecalChanStatus,const reco::SuperCluster* altSC);
};

struct ShowerShapeStruct {
  float e3x3,e5x5,seedClusEnergy,eMax,e2nd,eLeftRightDiffSumRatio,eTopBottomDiffSumRatio,sigmaIEtaIEta,sigmaIEtaIPhi,sigmaIPhiIPhi,e2x5Max,e2x5Top,e2x5Bottom,e2x5Left,e2x5Right,eTop,eBottom,eLeft,eRight;
  static std::string contents(){return "e3x3:e5x5:seedClusEnergy:eMax:e2nd:eLeftRightDiffSumRatio:eTopBottomDiffSumRatio:sigmaIEtaIEta:sigmaIEtaIPhi:sigmaIPhiIPhi:e2x5Max:e2x5Top:e2x5Bottom:e2x5Left:e2x5Right:eTop:eBottom:eLeft:eRight";}
  void clear(){
    e3x3=e5x5=seedClusEnergy=eMax=e2nd=eLeftRightDiffSumRatio=eTopBottomDiffSumRatio=sigmaIEtaIEta=sigmaIEtaIPhi=sigmaIPhiIPhi=e2x5Max=e2x5Top=e2x5Bottom=e2x5Left=e2x5Right=eTop=eBottom=eLeft=eRight=0.;
  }
  template<bool full5x5>
  void fill(const reco::CaloCluster& clus,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo);  
  void fill(const reco::GsfElectron::ShowerShape& eleSS,const reco::GsfElectron& ele);
  void fill(const reco::Photon::ShowerShape& phoSS);

};

struct EvtStruct {
  int runnr,lumiSec,eventnr;
  static std::string contents(){return "runnr/I:lumiSec:eventnr";}
  void clear(){runnr=lumiSec=eventnr=0;}
  void fill(const edm::Event& event);
};

struct GenInfoStruct {
  float energy,pt,eta,phi,pdgId,status,dR;
  static std::string contents(){return "energy/F:pt:eta:phi:pdgId:status:dR";}
  void clear(){energy=pt=eta=phi=pdgId=status=dR=0;}
  void fill(const reco::GenParticle& genPart, float iDR);
};

struct EGRegTreeStruct {
  int nrVert;
  float rho;
  float nrPUInt;
  float nrPUIntTrue;
  EvtStruct evt;
  SuperClustStruct sc;
  ShowerShapeStruct ssFull;
  ShowerShapeStruct ssFrac;
  EleStruct ele;
  PhoStruct pho;
  ShowerShapeStruct eleSSFull;
  ShowerShapeStruct phoSSFull;
  GenInfoStruct mc;
  ClustStruct clus1;
  ClustStruct clus2;
  ClustStruct clus3;
  void createBranches(TTree* tree);
  void setBranchAddresses(TTree* tree);
  void fill(const edm::Event& event,int iNrVert,float iRho,float nrPUInt,float nrTruePUInt,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo,const EcalChannelStatus& ecalChanStatus,const reco::SuperCluster* iSC,const reco::GenParticle* iMC,const reco::GsfElectron* iEle,const reco::Photon* iPho,const reco::SuperCluster* altSC);
  void clear(){
    nrVert=0;
    rho=0.;
    nrPUInt=0.;
    nrPUIntTrue=0.;
    evt.clear();
    sc.clear();
    ssFull.clear();
    ssFrac.clear();
    ele.clear();
    pho.clear();
    eleSSFull.clear();
    phoSSFull.clear();
    mc.clear();
    clus1.clear();
    clus2.clear();
    clus3.clear();
  }

};

template<bool full5x5>
void ShowerShapeStruct::fill(const reco::CaloCluster& clus,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo)
{
  const bool isEB = clus.seed().subdetId()==EcalBarrel;
  const EcalRecHitCollection& ecalHits = isEB ? ecalHitsEB : ecalHitsEE;

  e3x3 = EcalClusterToolsT<full5x5>::e3x3(clus,&ecalHits,&topo);
  e5x5 = EcalClusterToolsT<full5x5>::e5x5(clus,&ecalHits,&topo);
  eMax = EcalClusterToolsT<full5x5>::eMax(clus,&ecalHits);
  e2nd = EcalClusterToolsT<full5x5>::e2nd(clus,&ecalHits);
  eLeft = EcalClusterToolsT<full5x5>::eLeft(clus,&ecalHits,&topo);
  eRight = EcalClusterToolsT<full5x5>::eRight(clus,&ecalHits,&topo);
  const float eLeftRightSum  = eLeft + eRight;
  const float eLeftRightDiff  = eLeft - eRight;
  eLeftRightDiffSumRatio  = eLeftRightSum !=0.f ? eLeftRightDiff/eLeftRightSum : 0.f;
  eTop = EcalClusterToolsT<full5x5>::eTop(clus,&ecalHits,&topo);
  eBottom = EcalClusterToolsT<full5x5>::eBottom(clus,&ecalHits,&topo);
  const float eTopBottomSum  = eTop + eBottom;
  const float eTopBottomDiff  = eTop - eBottom;
  eTopBottomDiffSumRatio  = eTopBottomSum !=0.f ? eTopBottomDiff/eTopBottomSum : 0.f;

  e2x5Bottom =  EcalClusterToolsT<full5x5>::e2x5Bottom(clus,&ecalHits,&topo);
  e2x5Top =  EcalClusterToolsT<full5x5>::e2x5Top(clus,&ecalHits,&topo);
  e2x5Left =  EcalClusterToolsT<full5x5>::e2x5Left(clus,&ecalHits,&topo);
  e2x5Right =  EcalClusterToolsT<full5x5>::e2x5Right(clus,&ecalHits,&topo);
  e2x5Max = EcalClusterToolsT<full5x5>::e2x5Max(clus,&ecalHits,&topo);

  const auto localCovs =  EcalClusterToolsT<full5x5>::localCovariances(clus,&ecalHits,&topo);
 
  sigmaIEtaIEta =std::sqrt(localCovs[0]);
  sigmaIEtaIPhi = std::numeric_limits<float>::max();
  sigmaIPhiIPhi = std::numeric_limits<float>::max();
  if (!edm::isNotFinite(localCovs[2])) sigmaIPhiIPhi = std::sqrt(localCovs[2]) ;
  
  const bool applySPPBug = false;
  const float seeBySpp = applySPPBug ? sigmaIEtaIEta*std::numeric_limits<float>::max() : sigmaIEtaIEta*sigmaIPhiIPhi;
  
  if(  seeBySpp > 0 ) {
    sigmaIEtaIPhi = localCovs[1] / seeBySpp;
  } else if ( localCovs[1] > 0 ) {
    sigmaIEtaIPhi = 1.f;
  } else {
    sigmaIEtaIPhi = -1.f;
  }

}

#endif
