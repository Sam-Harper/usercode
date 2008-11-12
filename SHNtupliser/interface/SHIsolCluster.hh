#ifndef SHISOLCLUSTER
#define SHISOLCLUSTER


#include "TObject.h"
#include "TVector3.h"

#include <iostream>
#include <vector>

class SHIsolCluster : public TObject {
private:
  
  float nrgy_;
  TVector3 position_;
  //int nrCrys_;
  std::vector<int> hitDetIds_;

public:
  SHIsolCluster();
  SHIsolCluster(float nrgy,const TVector3& pos,const std::vector<int>& hitIds);
  ~SHIsolCluster(){}

   
  float nrgy()const{return nrgy_;}
  const TVector3& position()const{return position_;}
  int nrCrys()const{return hitDetIds_.size();}
  float eta()const{return position().Eta();}
  float phi()const{return position().Phi();}

private:
  ClassDef(SHIsolCluster,2)

};

#endif
