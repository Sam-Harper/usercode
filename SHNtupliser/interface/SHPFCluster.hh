#ifndef SHPFCLUSTER_HH
#define SHPFCLUSTER_HH

#include <vector>
#include <utility>
#include <cmath>
#include <iostream>
#include <string>

#include "TObject.h"
#include "TVector3.h"

namespace reco{
  class PFCluster;
}

class SHPFCluster {
public:

private:
  int superClusId_; //detId of seed cryst of sc it belongs too
  int caloId_;
  int flags_;
  int seedId_;
  
  float nrgy_;
  TVector3 pos_;

  std::vector<std::pair<int,float> > detIdAndFracs_;

public:
  SHPFCluster();
  SHPFCluster(const reco::PFCluster& clus,int scId);
  virtual ~SHPFCluster(){}

  int superClusId()const{return superClusId_;}
  int caloId()const{return caloId_;}
  int flags()const{return flags_;}
  int seedId()const{return seedId_;}

  float nrgy()const{return nrgy_;}
  float et()const{return nrgy()*sin(pos().Theta());}
  const TVector3& pos()const{return pos_;}

  std::ostream& print(std::ostream& out)const{out<<" et "<<et()<<" nrgy "<<nrgy()<<" eta "<<pos().Eta()<<" phi "<<pos().Phi()<<" caloId "<<caloId()<<" seedId "<<seedId()<<" flags "<<flags()<<" sc id "<<superClusId();return out;}
  friend std::ostream& operator<<(std::ostream& out,const SHPFCluster& clus){return clus.print(out);}

  ClassDef(SHPFCluster,1);

};



#endif
