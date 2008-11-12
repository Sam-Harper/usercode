#include "SHarper/SHNtupliser/interface/SHCaloHit.hh"

ClassImp(SHCaloHit)

SHCaloHit::SHCaloHit(int id,float nrgy):
  nrgy_(nrgy),
  detId_(id)
{

}

SHCaloHit::SHCaloHit(const SHCaloHit& rhs):
  nrgy_(rhs.nrgy_),
  detId_(rhs.detId_)
{

}


SHCaloHit& SHCaloHit::operator=(const SHCaloHit& rhs)
{
  nrgy_=rhs.nrgy_;
  detId_ = rhs.detId_;
  return *this;
}
