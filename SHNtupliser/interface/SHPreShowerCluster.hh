#ifndef SHPRESHOWERCLUSTER
#define SHPRESHOWERCLUSTER

#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"
#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"


#include <vector>
#include <iostream>


namespace reco{
  class PreshowerCluster;
}

class SHPreShowerCluster : public TObject {

 private:
  float nrgy_;
  TVector3 pos_;
  float eta_;
  int plane_;
  std::vector<int> hits_;
  
 public:
  SHPreShowerCluster();
  //  SHPreShowerCluster(const SHPreShowerCluster& rhs);
 
  SHPreShowerCluster(const reco::PreshowerCluster& clus);

  ~SHPreShowerCluster(){}

  float nrgy()const{return nrgy_;}
  float et()const{return nrgy()*pos().Perp()/pos().Mag();}
  
  const TVector3& position()const{return pos_;}
  const TVector3& pos()const{return pos_;}
  int nrHits()const{return hits_.size();}
  float eta()const{return eta_;}
  float phi()const{return position().Phi();}
  int plane()const{return plane_;}
  //  int seedCrys()const;
  
  const std::vector<int>& getHitsByDetId()const{return hits_;}
  friend std::ostream &operator <<(std::ostream& output,const SHPreShowerCluster &mcPart);
  std::ostream& print(std::ostream& output)const;
 private:


  ClassDef(SHPreShowerCluster,1)

};

#endif //SHPRESHOWERCLUSTER
  
