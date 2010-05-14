
#include "SHarper/SHNtupliser/interface/SHMuon.hh"

#include "DataFormats/MuonReco/interface/MuonIsolation.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/Track.h"
SHMuon::SHMuon(const reco::Muon& mu,const TVector3& beamPoint):
  p4_(mu.px(),mu.py(),mu.pz(),mu.energy()),
  eta_(p4_.Eta()),
  posCharge_(mu.charge()==1),
  type_(mu.type()),
  normChi2_(mu.globalTrack()->normalizedChi2()),
  // nrHits_(mu.globalTrack()->hitPattern().numberOfValidMuonHits()), 
  nrHits_(0),
  d0_(-999),
  isol03_(mu.isolationR03()),
  ecalDetId_(mu.calEnergy().ecal_id),
  hcalDetId_(mu.calEnergy().hcal_id),
  emCaloNrgy_(mu.calEnergy().em),
  hadCaloNrgy_(mu.calEnergy().had),
  hoCaloNrgy_(mu.calEnergy().ho)
{
  reco::TrackRef innerTrack = mu.innerTrack();
  if(innerTrack.isNonnull() && innerTrack.isAvailable()){
    nrHits_ = innerTrack->numberOfValidHits(); 
    reco::TrackBase::Point cmsswBeamPoint(beamPoint.X(),beamPoint.Y(),beamPoint.Z());
    d0_ = -1.0 * innerTrack->dxy( cmsswBeamPoint );
  }
}

SHMuon::MuIsolData::MuIsolData(const reco::MuonIsolation& muIso):
  sumPt(muIso.sumPt),
  hadEt(muIso.hadEt),
  hoEt(muIso.hoEt),
  nrTracks(muIso.nTracks),
  nrJets(muIso.nJets),
  trackerVetoPt(muIso.trackerVetoPt),
  emVetoEt(muIso.emVetoEt),
  hadVetoEt(muIso.hadVetoEt),
  hoVetoEt(muIso.hoVetoEt)
{

} 
