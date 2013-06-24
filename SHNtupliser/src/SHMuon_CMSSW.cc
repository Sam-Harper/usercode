
#include "SHarper/SHNtupliser/interface/SHMuon.hh"

#include "DataFormats/MuonReco/interface/MuonIsolation.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "DataFormats/MuonReco/interface/MuonCocktails.h"

SHMuon::SHMuon(const reco::Muon& mu,const TVector3& beamPoint):
  p4_(mu.px(),mu.py(),mu.pz(),mu.energy()), //resetting this lower down
  eta_(p4_.Eta()), //resetting this lower down
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
  //reco::TrackRef cktTrackRef = (muon::tevOptimized(mu, 200., 17., 40., 0.25)).first; // 53X 
  reco::TrackRef cktTrackRef = (muon::tevOptimized(mu)).first; //60X
  const reco::Track& cktTrack = *cktTrackRef;
  eta_=cktTrack.eta();
  p4_.SetPtEtaPhiM(cktTrack.pt(),cktTrack.eta(),cktTrack.phi(),0.1057);
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
