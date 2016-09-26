#include "SHarper/SHNtupliser/interface/SHIsolTrack.hh"

#include "DataFormats/TrackReco/interface/Track.h"

#include<iostream>

//grr, this should be member func of reco::Track
int getTrkQuality(const reco::Track& trk)
{
  int trkQual=0;
  for(int qualBit = 0;qualBit<reco::TrackBase::qualitySize;qualBit++){
    if(trk.quality(static_cast<reco::TrackBase::TrackQuality>(qualBit))) trkQual|=(0x1<<qualBit);
  }
  //now we deal with the invalid state
  if(trkQual==0) trkQual|=(0x1<<reco::TrackBase::qualitySize);
  return trkQual;
}

//note the seed cluster must be the first entry of the shape map
SHIsolTrack::SHIsolTrack(const reco::Track& trk,int vertexNr):
  p3_(trk.px(),trk.py(),trk.pz()),
  vtxPos_(trk.vx(),trk.vy(),trk.vz()),
  posCharge_(trk.charge()>0),
  vertexNr_(vertexNr),
  chi2_(trk.chi2()),
  ndof_(trk.ndof()),
  algosAndQual_(SHIsolTrack::packAlgoIDInfo(trk.algo(),trk.originalAlgo(),getTrkQuality(trk))),
  nrHits_(trk.hitPattern().trackerLayersWithMeasurement()),
  nrLostHits_(trk.hitPattern().trackerLayersWithoutMeasurement(reco::HitPattern::TRACK_HITS)),
  ptErr_(trk.ptError())
{

}
