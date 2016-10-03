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
  pt_(trk.pt()),eta_(trk.eta()),phi_(trk.phi()),
  vx_(trk.vx()),vy_(trk.vy()),vz_(trk.vz()),
  chi2_(trk.chi2()),
  ptErr_(trk.ptError()),
  algosAndQual_(SHIsolTrack::packAlgoIDInfo(trk.algo(),trk.originalAlgo(),getTrkQuality(trk))),
  trkData_(SHIsolTrack::packTrackData(trk.charge()>0,vertexNr,trk.ndof())),
  hitData_(SHIsolTrack::packHitData(trk.hitPattern().numberOfValidHits(),
				    trk.hitPattern().trackerLayersWithMeasurement(),
				    trk.hitPattern().trackerLayersWithoutMeasurement(reco::HitPattern::TRACK_HITS),
				    trk.hitPattern().numberOfValidPixelHits()))
{

}
