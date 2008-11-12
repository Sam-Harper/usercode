#ifndef SHBASICCLUSTER
#define SHBASICCLUSTER

#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"

#include "TObject.h"


#include <iostream>
#include <vector>

namespace reco{
  class BasicCluster;
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

  
  std::vector<int> hitDetIds_;

 public:
  SHBasicCluster();
  SHBasicCluster(const SHBasicCluster& rhs);
  SHBasicCluster(const reco::BasicCluster& clus,const SHCaloHitContainer& hits);

  ~SHBasicCluster(){}

  //float eMax()const{return eMax_;}
  //float e2nd()const{return e2nd_;} 
  // float e2x2()const{return e2x2_;} 
  //float e3x2()const{return e3x2_;} 
  //float e3x3()const{return e3x3_;} 
  //float e4x4()const{return e4x4_;} 
  //float e5x5()const{return e5x5_;} 
  float totNrgy()const{return totNrgy_;}
  int nrCrys()const{return nrCrys_;}
  int eMaxId()const{return eMaxId_;}
  int e2ndId()const{return e2ndId_;}

  int crysDetId(int crysNr)const{return hitDetIds_[crysNr];}
  int nrCrysStored()const{return hitDetIds_.size();}

  const std::vector<int>& getHitsByDetId()const{return hitDetIds_;}

 private:
  //SHBasicCluster& operator=(const SHBasicCluster &rhs){return *this;}

  ClassDef(SHBasicCluster,4)

};


#endif
