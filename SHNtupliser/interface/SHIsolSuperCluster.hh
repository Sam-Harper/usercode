#ifndef SHISOLSUPERCLUSTER
#define SHISOLSUPERCLUSTER

#include "TObject.h"
#include "TVector3.h"

#include <iostream>
#include <vector>

class SHIsolSuperCluster : public TObject {
private:
  
  float nrgy_;
  float rawNrgy_;
  TVector3 position_;
  std::vector<int> basicClusIndex_;

public:
  SHIsolSuperCluster();
  SHIsolSuperCluster(float nrgy,float rawNrgy,const TVector3& pos);
  ~SHIsolSuperCluster(){}

  void addBasicCluster(int index){basicClusIndex_.push_back(index);}

  int getClusNr(int index)const{return basicClusIndex_[index];}
  int nrClus()const{return basicClusIndex_.size();}
  const TVector3& position()const{return position_;}
  float nrgy()const{return nrgy_;}
  float rawNrgy()const{return rawNrgy_;}

  ClassDef(SHIsolSuperCluster,1)

};

#endif
  
