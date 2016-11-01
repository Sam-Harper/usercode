#ifndef SHSUPERCLUSTER
#define SHSUPERCLUSTER

#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"
#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"
#include "SHarper/SHNtupliser/interface/SHPreShowerCluster.hh"

#include "SHarper/SHNtupliser/interface/MathFuncs.hh"

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"


#include <vector>
#include <iostream>


namespace reco{
  class SuperCluster;
  class ClusterShape;
}

class SHSuperCluster : public TObject {

 private:
  float nrgy_;
  float preShowerNrgy_;
  //  TVector3 pos_;
  float rho_;
  float eta_;
  float phi_;
  int nrCrys_;
  TClonesArray clusterArray_;
  //new for version 5
  int flags_;
  //  TClonesArray preShowerArray_;
 public:
  SHSuperCluster();
  SHSuperCluster(const SHSuperCluster& rhs);
  //note that in shape map, the seed should be the first entry
  //note: change pointers to reference variables
  SHSuperCluster(const reco::SuperCluster& superClus);
  SHSuperCluster(const std::vector<SHBasicCluster>& basicClusters);
  ~SHSuperCluster();//need to check if need to delete TClonesArray objects
 
  float nrgy()const{return nrgy_;}
  float et()const{return nrgy()*std::sin(theta());}
  float rawNrgy()const;
  float preShowerNrgy()const{return preShowerNrgy_;}
  TVector3 pos()const{TVector3 posVec;posVec.SetMagThetaPhi(rho(),theta(),phi());return posVec;}
  TVector3 position()const{return pos();}
  int nrCrys()const{return nrCrys_;}
  int nrClus()const{return clusterArray_.GetLast()+1;}
  //int nrPreShowerClus()const{return preShowerArray_.GetLast()+1;}
  float rho()const{return rho_;}
  float eta()const{return eta_;} 
  float theta()const{return MathFuncs::etaToTheta(eta());}
  float phi()const{return phi_;}
 
  //  int seedCrys()const;
  
  const SHBasicCluster* seedClus()const;
  const SHBasicCluster* getClus(int clusNr)const; //clusNr=0 always seed cluster
  // const SHPreShowerCluster* getPreShowerClus(int clusNr)const;
  
  friend std::ostream &operator <<(std::ostream& output,const SHSuperCluster &clus);
  std::ostream& print(std::ostream& output)const;

 private: 


  ClassDef(SHSuperCluster,7)

};

#endif //SHSUPERCLUSTER
  
