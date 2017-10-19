#include "SHarper/SHNtupliser/interface/SHPFCandidate.hh"


float SHPFCandidate::dxy(const TVector3& pos)const
{
  float dxy = ( -(vx() - pos.X())*py() + (vy() - pos.Y())*px()) / pt();
  return dxy;
}

std::ostream& SHPFCandidate::print(std::ostream& out)const
{
  out<<"pt: "<<pt()<<" eta "<<eta()<<" phi "<<phi()<<" mass "<<mass()<<" hadNrgyRaw "<<hadNrgyRaw();
  return out;
}


ClassImp(SHPFCandidate)
