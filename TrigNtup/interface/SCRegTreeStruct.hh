#ifndef SHarper_TrigNtup_SCRegTreeStruct_hh
#define SHarper_TrigNtup_SCRegTreeStruct_hh

#include "FWCore/Framework/interface/Event.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

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


struct SuperClustStruct {
  float rawEnergy,etaWidth,phiWidth,e3x3,seedClusEnergy,eMax,e2nd,eLeftRightDiffSumRatio,eTopBottomDiffSumRatio,sigmaIEtaIEta,sigmaIEtaIPhi,sigmaIPhiIPhi,numberOfClusters,clusterMaxDR,clusterMaxDRDPhi,clusterMaxDRDEta,clusterMaxDRRawEnergy,iEtaOrX,iPhiOrY,isEB,corrEnergy74X,scEta,scPhi,seedEta,seedPhi;
  static std::string contents(){return "rawEnergy/F:etaWidth:phiWidth:e3x3:seedClusEnergy:eMax:e2nd:eLeftRightDiffSumRatio:eTopBottomDiffSumRatio:sigmaIEtaIEta:sigmaIEtaIPhi:sigmaIPhiIPhi:numberOfClusters:clusterMaxDR:clusterMaxDRDPhi:clusterMaxDRDEta:clusterMaxDRRawEnergy:iEtaOrX:iPhiOrY:isEB:corrEnergy74X:scEta:scPhi:seedEta:seedPhi";}
  void clear(){
    rawEnergy=etaWidth=phiWidth=e3x3=seedClusEnergy=eMax=e2nd=eLeftRightDiffSumRatio=eTopBottomDiffSumRatio=sigmaIEtaIEta=sigmaIEtaIPhi=sigmaIPhiIPhi=numberOfClusters=clusterMaxDR=clusterMaxDRDPhi=clusterMaxDRDEta=clusterMaxDRRawEnergy=iEtaOrX=iPhiOrY=isEB=corrEnergy74X=scEta=scPhi=seedEta=seedPhi=0;
  }

  void fill(const reco::SuperCluster& sc,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo);
};


struct EvtStruct {
  int runnr,lumiSec,eventnr;
  static std::string contents(){return "runnr/I:lumiSec:eventnr";}
  void clear(){runnr=lumiSec=eventnr=0;}
  void fill(const edm::Event& event);
};

struct GenInfo {
  float energy,pt,eta,phi,pdgId,status,dR;
  static std::string contents(){return "energy/F:pt:eta:phi:pdgId:status:dR";}
  void clear(){energy=pt=eta=phi=pdgId=status=dR=0;}
  void fill(const reco::GenParticle& genPart, float iDR);
};

struct SCRegTreeStruct {
  int nrVert;
  EvtStruct evt;
  SuperClustStruct sc;
  GenInfo mc;
  ClustStruct clus1;
  ClustStruct clus2;
  ClustStruct clus3;


  void createBranches(TTree* tree);
  void setBranchAddresses(TTree* tree);
  void fill(const edm::Event& event,int iNrVert,const reco::SuperCluster& iSC,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo,const reco::GenParticle* iMC = nullptr);
  
};



#endif
