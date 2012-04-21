#ifndef SHBASICCLUSTER
#define SHBASICCLUSTER

#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"

#include "TObject.h"
#include "TVector3.h"

#include <iostream>
#include <vector>
#include <cmath>

namespace reco{
  class CaloCluster;
}

class SHBasicCluster : public TObject {

 private:
  //  float eMax_;
  //float e2nd_; 
  //float e2x2_; 
  //float e3x2_; 
  //float e3x3_; 
  //float e4x4_; 
  //float e5x5_; 
  //float e2x5Right_; 
  //float e2x5Left_; 
  //float e2x5Top_; 
  //float e2x5Bottom_; 
  //float e3x2Ratio_;
  float totNrgy_;
  int nrCrys_;
  int eMaxId_;
  int e2ndId_;
  float eta_;
  float phi_;

  
  mutable std::vector<int> hitDetIds_; //think of it like a temporary cache
  int seedId_;
  //packed info telling us about the hits used by the cluster
  //used to populate hitDetIds,saves a lot of space...
  //format: bit 0: 0 EB, 1 EE
  //if EE: bits 1-26 represent the 5x5 cluster : 0 hit used, 1 hit not, starts at -2,-2
  //if EB: bits 1-6: -ve phi road, 7-12 +ve phi road
  int hitInfo_; 
  

 public:
  SHBasicCluster();
  SHBasicCluster(const SHBasicCluster& rhs);
  SHBasicCluster(const reco::CaloCluster& clus,const SHCaloHitContainer& hits);
  SHBasicCluster(float nrgy,int seedId,int e2ndId,float eta,float phi,const std::vector<int>& hits);
  ~SHBasicCluster(){}

  //float eMax()const{return eMax_;}
  //float e2nd()const{return e2nd_;} 
  // float e2x2()const{return e2x2_;} 
  //float e3x2()const{return e3x2_;} 
  //float e3x3()const{return e3x3_;} 
  //float e4x4()const{return e4x4_;} 
  //float e5x5()const{return e5x5_;} 
  float totNrgy()const{return totNrgy_;}
  float nrgy()const{return totNrgy_;}
  float et()const{return sin(pos().Theta())*totNrgy_;}
  int nrCrys()const{return nrCrys_;}
  int eMaxId()const{return eMaxId_==0 ? seedId(): eMaxId_;} //transition to dumping emax
  int e2ndId()const{return e2ndId_;}
  int seedId()const{return seedId_;} //seed!=eMax 100% of the time in future
  
  float eta()const{return eta_;}
  float phi()const{return phi_;}
  
  TVector3 pos()const{TVector3 thePos;thePos.SetPtEtaPhi(1.,eta_,phi_);return thePos;}
  int crysDetId(int crysNr)const{unpackHits_();return hitDetIds_[crysNr];}
  int nrCrysStored()const{unpackHits_();return hitDetIds_.size();}

  const std::vector<int>& getHitsByDetId()const{unpackHits_();return hitDetIds_;}

 private:
  SHBasicCluster& operator=(const SHBasicCluster &rhs){return *this;}
  static int packHits_(int seedId,const std::vector<int>& hits);
  bool unpackHits_(bool forceUnpack=false)const;

  ClassDef(SHBasicCluster,5)

};


#endif
