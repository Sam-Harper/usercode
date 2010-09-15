#include "SHarper/SHNtupliser/interface/SHCaloHit.hh"

ClassImp(SHCaloHit)

SHCaloHit::SHCaloHit(int id,float nrgy):
  nrgy_(nrgy),
  detId_(id),
  time_(0),
  flag_(0)
{

}

SHCaloHit::SHCaloHit(int id,float nrgy,float time,uint32_t flag):
  nrgy_(nrgy),
  detId_(id),
  time_(time),
  flag_(flag)
{

}

// SHCaloHit::SHCaloHit(const SHCaloHit& rhs):
//   nrgy_(rhs.nrgy_),
//   detId_(rhs.detId_)
// {

// }


// SHCaloHit& SHCaloHit::operator=(const SHCaloHit& rhs)
// {
//   nrgy_=rhs.nrgy_;
//   detId_ = rhs.detId_;
//   return *this;
// }
