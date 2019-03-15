#ifndef SHarper_TrigNtup_SCRegTreeStruct_hh
#define SHarper_TrigNtup_SCRegTreeStruct_hh

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/isFinite.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"

#include "TTree.h"

namespace reco{
  class SuperCluster;
  class GenParticle;
  class GsfElectron;
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
  float et,energy,eta,phi,trkEtaMode,trkPhiMode,trkPMode,trkPModeErr,fbrem,corrMean,corrSigma,hademTow,hademCone;
  static std::string contents(){return "et/F:energy:eta:phi:trkEtaMode:trkPhiMode:trkPMode:trkPModeErr:fbrem:corrMean:corrSigma:hademTow:hademCone";}
  void clear(){et=energy=eta=phi=trkEtaMode=trkPhiMode=trkPMode=trkPModeErr=fbrem=corrMean=corrSigma=hademTow=hademCone=0.;}
  void fill(const reco::GsfElectron& ele);
};

struct SuperClustStruct {
  float rawEnergy,rawESEnergy,etaWidth,phiWidth,seedClusEnergy,numberOfClusters,numberOfSubClusters,clusterMaxDR,clusterMaxDRDPhi,clusterMaxDRDEta,clusterMaxDRRawEnergy,iEtaOrX,iPhiOrY,isEB,corrEnergy74X,scEta,scPhi,seedEta,seedPhi;
  static std::string contents(){return "rawEnergy/F:rawESEnergy:etaWidth:phiWidth:seedClusEnergy:numberOfClusters:numberOfSubClusters:clusterMaxDR:clusterMaxDRDPhi:clusterMaxDRDEta:clusterMaxDRRawEnergy:iEtaOrX:iPhiOrY:isEB:corrEnergy74X:scEta:scPhi:seedEta:seedPhi";}
  void clear(){
    rawEnergy=rawESEnergy=etaWidth=phiWidth=seedClusEnergy=numberOfClusters=numberOfSubClusters=clusterMaxDR=clusterMaxDRDPhi=clusterMaxDRDEta=clusterMaxDRRawEnergy=iEtaOrX=iPhiOrY=isEB=corrEnergy74X=scEta=scPhi=seedEta=seedPhi;
  }

  void fill(const reco::SuperCluster& sc);
};

struct ShowerShapeStruct {
  float e3x3,seedClusEnergy,eMax,e2nd,eLeftRightDiffSumRatio,eTopBottomDiffSumRatio,sigmaIEtaIEta,sigmaIEtaIPhi,sigmaIPhiIPhi;
  static std::string contents(){return "e3x3:seedClusEnergy:eMax:e2nd:eLeftRightDiffSumRatio:eTopBottomDiffSumRatio:sigmaIEtaIEta:sigmaIEtaIPhi:sigmaIPhiIPhi";}
  void clear(){
    e3x3=seedClusEnergy=eMax=e2nd=eLeftRightDiffSumRatio=eTopBottomDiffSumRatio=sigmaIEtaIEta=sigmaIEtaIPhi=sigmaIPhiIPhi=0.;
  }
  template<bool full5x5>
  void fill(const reco::CaloCluster& clus,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo);  
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
  EvtStruct evt;
  SuperClustStruct sc;
  EleStruct ele;
  GenInfoStruct mc;
  ClustStruct clus1;
  ClustStruct clus2;
  ClustStruct clus3;
  void createBranches(TTree* tree);
  void setBranchAddresses(TTree* tree);
  void fill(const edm::Event& event,int iNrVert,float iRho,const reco::SuperCluster& iSC,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo,const reco::GenParticle* iMC,const reco::GsfElectron* iEle);
  void clear(){
    nrVert=0;
    rho=0.;
    evt.clear();
    sc.clear();
    ele.clear();
    mc.clear();
    clus1.clear();
    clus2.clear();
    clus3.clear();
  }

};

template<bool full5x5>
void ShowerShapeStruct::fill(const reco::CaloCluster& clus,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo)
{
  bool isEB = clus.seed().subdetId()==EcalBarrel;
  const EcalRecHitCollection& ecalHits = isEB ? ecalHitsEB : ecalHitsEE;

  e3x3 = EcalClusterToolsT<full5x5>::e3x3(clus,&ecalHits,&topo);
  eMax = EcalClusterToolsT<full5x5>::eMax(clus,&ecalHits);
  e2nd = EcalClusterToolsT<full5x5>::e2nd(clus,&ecalHits);
  float eLeft = EcalClusterToolsT<full5x5>::eLeft(clus,&ecalHits,&topo);
  float eRight = EcalClusterToolsT<full5x5>::eRight(clus,&ecalHits,&topo);
  float eLeftRightSum  = eLeft + eRight;
  float eLeftRightDiff  = eLeft - eRight;
  eLeftRightDiffSumRatio  = eLeftRightSum !=0. ? eLeftRightDiff/eLeftRightSum : 0.;
  float eTop = EcalClusterToolsT<full5x5>::eTop(clus,&ecalHits,&topo);
  float eBottom = EcalClusterToolsT<full5x5>::eBottom(clus,&ecalHits,&topo);
  float eTopBottomSum  = eTop + eBottom;
  float eTopBottomDiff  = eTop - eBottom;
  eTopBottomDiffSumRatio  = eTopBottomSum !=0. ? eTopBottomDiff/eTopBottomSum : 0.;
  auto localCovs =  EcalClusterToolsT<full5x5>::localCovariances(clus,&ecalHits,&topo);
 
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
