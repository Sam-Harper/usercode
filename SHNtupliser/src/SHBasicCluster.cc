#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"
  
ClassImp(SHBasicCluster)

SHBasicCluster::SHBasicCluster():
  totNrgy_(0.),
  nrCrys_(0),
  eMaxId_(0),
  e2ndId_(0),
  eta_(0.),
  phi_(0.),
  hitDetIds_()
{

}


SHBasicCluster::SHBasicCluster(const SHBasicCluster& rhs):
  totNrgy_(rhs.totNrgy_),
  nrCrys_(rhs.nrCrys_),
  eMaxId_(rhs.eMaxId_),
  e2ndId_(rhs.e2ndId_),
  eta_(rhs.eta_),
  phi_(rhs.phi_),
  hitDetIds_(rhs.hitDetIds_)
{

}

