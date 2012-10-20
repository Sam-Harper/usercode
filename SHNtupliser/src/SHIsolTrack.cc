#include "SHarper/SHNtupliser/interface/SHIsolTrack.hh"

ClassImp(SHIsolTrack)

SHIsolTrack::SHIsolTrack():
  p3_(0.,0.,0.),
  vtxPos_(0.,0.,0.),
  posCharge_(false),
  vertexNr_(-1),
  chi2_(-999),
  ndof_(-1)
{

}

SHIsolTrack::SHIsolTrack(const TVector3& p3,const TVector3& vtxPos,bool posCharge,int vertexNr,float chi2,int ndof):
  p3_(p3),
  vtxPos_(vtxPos),
  posCharge_(posCharge),
  vertexNr_(vertexNr),
  chi2_(chi2),
  ndof_(ndof)
{

}


float SHIsolTrack::dz()const
{
  return vtxPos().Z()-(vtxPos().X()*p3().X()+vtxPos().Y()*p3().Y())/p3().Pt() * p3().Z()/p3().Pt();
}
