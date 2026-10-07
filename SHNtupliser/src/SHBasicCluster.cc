#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"
  
#include "SHarper/SHNtupliser/interface/LogErr.hh"
#include <algorithm>
ClassImp(SHBasicCluster)

SHBasicCluster::SHBasicCluster():
  totNrgy_(0.),
  nrCrys_(0),
  r_(0.),
  eta_(0.),
  phi_(0.),
  seedId_(0)
{

}


SHBasicCluster::SHBasicCluster(const SHBasicCluster& rhs):TObject(rhs),
  totNrgy_(rhs.totNrgy_),
  nrCrys_(rhs.nrCrys_),
  r_(rhs.r_),
  eta_(rhs.eta_),
  phi_(rhs.phi_),
  seedId_(rhs.seedId_)
{
  
}

SHBasicCluster::SHBasicCluster(float nrgy,int seedId,float r,float eta,float phi,int nrCrys):
  totNrgy_(nrgy),
  nrCrys_(nrCrys),
  r_(r),
  eta_(eta),
  phi_(phi),
  seedId_(seedId)
{

}

