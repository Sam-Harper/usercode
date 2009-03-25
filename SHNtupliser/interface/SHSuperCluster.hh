#ifndef SHSUPERCLUSTER
#define SHSUPERCLUSTER

#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"
#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"


#include <vector>
#include <iostream>


namespace reco{
  class SuperCluster;
  class BasicCluster;
  class ClusterShape;
}

class SHSuperCluster : public TObject {

 private:
  float nrgy_;
  float preShowerNrgy_;
  TVector3 pos_;
  float eta_;
  int nrCrys_;
  TClonesArray clusterArray_;
 
 public:
  SHSuperCluster();
  SHSuperCluster(const SHSuperCluster& rhs);
  //note that in shape map, the seed should be the first entry
  //note: change pointers to reference variables
  SHSuperCluster(const reco::SuperCluster& superClus,const SHCaloHitContainer& hits);
  ~SHSuperCluster();//need to check if need to delete TClonesArray objects


  float nrgy()const{return nrgy_;}
  float et()const{return nrgy()*pos().Perp()/pos().Mag();}
  float rawNrgy()const;
  float preShowerNrgy()const{return preShowerNrgy_;}
  const TVector3& position()const{return pos_;}
  const TVector3& pos()const{return pos_;}
  int nrCrys()const{return nrCrys_;}
  int nrClus()const{return clusterArray_.GetLast()+1;}
  float eta()const{return eta_;}
  float phi()const{return position().Phi();}
  //  int seedCrys()const;
  
  const SHBasicCluster* seedClus()const;
  const SHBasicCluster* getClus(int clusNr)const; //clusNr=0 always seed cluster
 
  
  void getHitsByDetId(std::vector<int>& hitDetIds)const;

 private:


  ClassDef(SHSuperCluster,4)

};

#endif //SHSUPERCLUSTER
  
