#include "SHarper/SHNtupliser/interface/SHIsolCluster.hh"

ClassImp(SHIsolCluster)

SHIsolCluster::SHIsolCluster():
  nrgy_(0.),
  position_(0.,0.,0.),
  hitDetIds_()
  //nrCrys_(0)
{

}


SHIsolCluster::SHIsolCluster(float nrgy,const TVector3& pos,const std::vector<int>& hitDetIds):
  nrgy_(nrgy),
  position_(pos),
  hitDetIds_(hitDetIds)
  //nrCrys_(nrCrys)
{

}
