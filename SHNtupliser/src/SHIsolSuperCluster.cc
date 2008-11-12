#include "SHarper/SHNtupliser/interface/SHIsolSuperCluster.hh"

ClassImp(SHIsolSuperCluster)

SHIsolSuperCluster::SHIsolSuperCluster():
  nrgy_(0.),
  rawNrgy_(0.),
  position_(0.,0.,0.),
  basicClusIndex_()
{

}

SHIsolSuperCluster::SHIsolSuperCluster(float nrgy,float rawNrgy,const TVector3& pos):
  nrgy_(nrgy),
  rawNrgy_(rawNrgy),
  position_(pos),
  basicClusIndex_()
{

}

