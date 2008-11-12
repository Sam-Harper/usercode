#include "SHarper/SHNtupliser/interface/SHIsolTrack.hh"

ClassImp(SHIsolTrack)

SHIsolTrack::SHIsolTrack():
  p3_(0.,0.,0.),
  vtxPos_(0.,0.,0.),
  posCharge_(false)
{

}

SHIsolTrack::SHIsolTrack(const TVector3& p3,const TVector3& vtxPos,bool posCharge):
  p3_(p3),
  vtxPos_(vtxPos),
  posCharge_(posCharge)
{

}



SHIsolTrack::SHIsolTrack(const SHIsolTrack& rhs):
  p3_(rhs.p3_),
  vtxPos_(rhs.vtxPos_),
  posCharge_(rhs.posCharge_)
{

}

float SHIsolTrack::dz()const
{
  return vtxPos().Z()-(vtxPos().X()*p3().X()+vtxPos().Y()*p3().Y())/p3().Pt() * p3().Z()/p3().Pt();
}
