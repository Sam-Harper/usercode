#include "SHarper/SHNtupliser/interface/SHIsolTrack.hh"

ClassImp(SHIsolTrack)

SHIsolTrack::SHIsolTrack():
  pt_(0.),eta_(0.),phi_(0.),
  vx_(0.),vy_(0.),vz_(0.),
  chi2_(-999.),
  ptErr_(0.),
  algosAndQual_(0),
  trkData_(0),
  hitData_(0)
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
  TVector3 mom;
  mom.SetPtEtaPhi(pt(),eta(),phi());
  return vz()-(vx()*mom.X()+vy()*mom.Y())/mom.Pt() * mom.Z()/mom.Pt();
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

int SHIsolTrack::packTrackData(bool posCharge,int vertexNr,int ndof)
{
  int packedData=0;
  packedData |= (posCharge&kChargeMask) << kChargeOffset;
  packedData |= (vertexNr&kVertexNrMask) << kVertexNrOffset;
  packedData |= (ndof&kNDOFMask) << kNDOFOffset;
  return packedData;
}

int SHIsolTrack::packHitData(int nrHits,int nrLayers,int nrLostHits,int nrPixelHits)
{
  int packedData=0;
  packedData |= (nrHits&kNrHitsMask) << kNrHitsOffset;
  packedData |= (nrLayers&kNrLayersMask) << kNrLayersOffset;
  packedData |= (nrLostHits&kNrLostHitsMask) << kNrLostHitsOffset;
  packedData |= (nrPixelHits&kNrPixelHitsMask) << kNrPixelHitsOffset;
  return packedData;

}
