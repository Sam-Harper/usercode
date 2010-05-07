#ifndef SHMUON
#define SHMUON

#include "TObject.h"
#include "TLorentzVector.h"


namespace reco{
  class Muon;
  class MuonIsolation;

}


class SHMuon : public TObject {
public:
  struct MuIsolData {
    float sumPt;
    float hadEt;
    float hoEt;
    int nrTracks;
    int nrJets;
    float trackerVetoPt;
    float emVetoEt;
    float hadVetoEt;
    float hoVetoEt;

    MuIsolData();
    MuIsolData(const reco::MuonIsolation& muIso);
   
  };

private:
  TLorentzVector p4_;
  float eta_; //cached for speed, identical to that stored in p4_;
  bool posCharge_;

  int type_; //global muon, tracker muon, etc
  float normChi2_;
  int nrHits_;
  float d0_;

  
  //isolation
  MuIsolData isol03_;
  
  //calo information
  int ecalDetId_;
  int hcalDetId_;
  float emCaloNrgy_;
  float hadCaloNrgy_;
  float hoCaloNrgy_;

public:
  SHMuon();
  //dont own anything so operator=, copy construct not needed
  ~SHMuon(){}

  SHMuon(const reco::Muon& mu,const TVector3& beamSpot);

  const TLorentzVector& p4()const{return p4_;}
  float eta()const{return eta_;}
  int charge()const{return posCharge_ ? 1 : -1;}
  int type()const{return type_;}
  float normChi2()const{return normChi2_;}
  int nrHits()const{return nrHits_;}
  float d0()const{return d0_;}
  const MuIsolData& isol03()const{return isol03_;}
  int ecalDetId()const{return ecalDetId_;}
  int hcalDetId()const{return hcalDetId_;}
  float emCaloNrgy()const{return emCaloNrgy_;}
  float hadCaloNrgy()const{return hadCaloNrgy_;}
  float hoCaloNrgy()const{return hoCaloNrgy_;}


  ClassDef(SHMuon,1)

};


#endif
