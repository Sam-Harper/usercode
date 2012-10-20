#include "SHarper/SHNtupliser/interface/SHPFCandidate.hh"

float SHPFCandidate::dxy(const TVector3& pos)const
{
  float dxy = ( -(vx() - pos.X())*py() + (vy() - pos.Y())*px()) / pt();
  return dxy;
}

ClassImp(SHPFCandidate)
