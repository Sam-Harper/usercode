#include "SHarper/SHNtupliser/interface/SHCaloHit.hh"

ClassImp(SHCaloHit)

SHCaloHit::SHCaloHit(int id,float nrgy):
  nrgy_(nrgy),
  detId_(id),
  time_(0),
  flag_(0),
  flagBits_(0)
{

}

SHCaloHit::SHCaloHit(int iId,float iNrgy,float iTime,uint32_t iFlag,uint32_t iFlagBits):
  nrgy_(iNrgy),
  detId_(iId),
  time_(iTime),
  flag_(iFlag),
  flagBits_(iFlagBits)
{

}

bool operator<(int lhs,const SHCaloHit& rhs)
{
  return lhs<rhs.detId_;
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
