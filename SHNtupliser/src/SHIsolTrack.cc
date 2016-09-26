#include "SHarper/SHNtupliser/interface/SHIsolTrack.hh"

ClassImp(SHIsolTrack)

SHIsolTrack::SHIsolTrack():
  p3_(0.,0.,0.),
  vtxPos_(0.,0.,0.),
  posCharge_(false),
  vertexNr_(-1),
  chi2_(-999),
  ndof_(-1),
  algosAndQual_(0),
  nrHits_(0),
  nrLostHits_(0),
  ptErr_(0.)
  
{

}

// SHIsolTrack::SHIsolTrack(const TVector3& p3,const TVector3& vtxPos,bool posCharge,int vertexNr,float chi2,int ndof,int algosAndQual):
//   p3_(p3),
//   vtxPos_(vtxPos),
//   posCharge_(posCharge),
//   vertexNr_(vertexNr),
//   chi2_(chi2),
//   ndof_(ndof),
//   algosAndQual_(algosAndQual)
// {

// }


float SHIsolTrack::dz()const
{
  return vtxPos().Z()-(vtxPos().X()*p3().X()+vtxPos().Y()*p3().Y())/p3().Pt() * p3().Z()/p3().Pt();
}

int SHIsolTrack::packAlgoIDInfo(int algoId,int prevAlgoId,int quality)
{
  int packedId=0;
  packedId |= (algoId&kAlgoIdMask);
  packedId |= (prevAlgoId&kAlgoIdMask)<<kAlgoIdSize;
  packedId |= (quality&kQualMask)<<kAlgoIdSize*2;
  return packedId;
}


void SHIsolTrack::unpackAlgoIDInfo(const int packedId,int& algoId,int& prevAlgoId,int& quality)
{
  algoId = packedId&kAlgoIdMask;
  prevAlgoId = (packedId>>kAlgoIdSize)&kAlgoIdMask;
  quality = (packedId>>kAlgoIdSize*2)&kQualMask;
}
