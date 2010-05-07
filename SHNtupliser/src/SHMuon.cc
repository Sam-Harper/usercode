#include "SHarper/SHNtupliser/interface/SHMuon.hh"

ClassImp(SHMuon)

SHMuon::SHMuon():
  p4_(-999),
  eta_(-999),
  posCharge_(-999),
  type_(-999),
  normChi2_(-999),
  nrHits_(-999),
  d0_(-999),
  isol03_(),
  ecalDetId_(-999),
  hcalDetId_(-999),
  emCaloNrgy_(-999),
  hadCaloNrgy_(-999),
  hoCaloNrgy_(-999)
{

}


SHMuon::MuIsolData::MuIsolData():
  sumPt(-999),
  hadEt(-999),
  hoEt(-999),
  nrTracks(-999),
  nrJets(-999),
  trackerVetoPt(-999),
  emVetoEt(-999),
  hadVetoEt(-999),
  hoVetoEt(-999)
{

}
