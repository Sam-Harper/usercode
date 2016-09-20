#ifndef SHBASICCLUSTER
#define SHBASICCLUSTER

#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <iostream>
#include <vector>
#include <cmath>

namespace reco{
  class CaloCluster;
}

class SHBasicCluster : public TObject {

 private:
  float totNrgy_;
  int nrCrys_;
  float eta_;
  float phi_;
  int seedId_;
  
 public:
  SHBasicCluster();
  SHBasicCluster(const SHBasicCluster& rhs);
  SHBasicCluster(const reco::CaloCluster& clus);
  SHBasicCluster(float nrgy,int seedId,float eta,float phi,int nrCrys);
  ~SHBasicCluster(){}

  float totNrgy()const{return totNrgy_;}
  float nrgy()const{return totNrgy_;}
  float et()const{return sin(pos().Theta())*totNrgy_;}
  int nrCrys()const{return nrCrys_;}
  int seedId()const{return seedId_;} 
  
  float eta()const{return eta_;}
  float phi()const{return phi_;}
  
  TVector3 pos()const{TVector3 thePos;thePos.SetPtEtaPhi(1.,eta_,phi_);return thePos;}
  TLorentzVector p4()const{TLorentzVector theP4;theP4.SetPtEtaPhiM(et(),eta(),phi(),0);return theP4;}

 private:
  SHBasicCluster& operator=(const SHBasicCluster &){return *this;}

  ClassDef(SHBasicCluster,6)

};


#endif
