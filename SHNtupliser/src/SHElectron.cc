#include "SHarper/SHNtupliser/interface/SHElectron.hh"
#include "SHarper/SHNtupliser/interface/SHEvent.hh"

#include "SHarper/SHNtupliser/interface/MathFuncs.hh"

ClassImp(SHElectron)

SHElectron::SHElectron():
  //classification variables
  type_(0),
  //kinematic quantities
  p4_(0.,0.,0.,0.),
  et_(0.),
  nrgy_(0.),
  nrgyErr_(0.),
  rawNrgy_(0.),
  preShowerNrgy_(0.),
  epCombNrgy_(-999.),
  phoNrgy_(0.),
  altNrgy_(0.),
  altNrgyErr_(0.),
  altEPCombNrgy_(0.),
  posCal_(0.,0.,0.),
  e5x5_(0.),
  r9_(0.),
  eta_(0.),
  detEta_(0.),
//track quantities (momentum + positions)
//momemtums
  p3TrackVtx_(0.,0.,0.),
  p3TrackCal_(0.,0.,0.),
  p3TrackInn_(0.,0.,0.),
  p3TrackOut_(0.,0.,0.),
//positions
  posTrackVtx_(0.,0.,0.),
  posTrackCal_(0.,0.,0.),
  posTrackInn_(0.,0.,0.),
  posTrackOut_(0.,0.,0.),
  trkChi2_(0.),
  nrDof_(0),
  posCharge_(false),
  d0_(999.),
  trkMomErr_(0.),
//id quantities
  epIn_(0.),
  epOut_(0.),
  hadem_(0.),
  hademDepth1_(0.),
  hademDepth2_(0.),
  dEtaIn_(999.),
  dEtaOut_(999.),
  dPhiIn_(999.),
  dPhiOut_(999.),
  sigmaEtaEta_(0.),
  sigmaIEtaIEta_(0.),
  sigmaIPhiIPhi_(0.),
//links to tracks, superClusters
  superClusIndx_(-1),
  //new isolation variables
  isolEm_(999.),
  isolHad_(999.),
  isolHadDepth1_(999.),
  isolHadDepth2_(999.),
  isolPtTrks_(999.),
  isolNrTrks_(999),
  cutCode_(~0x0),
  cutCodeVID_(~0x0),
  e1x5Over5x5_(-999.),
  e2x5Over5x5_(-999.), 
  isEcalDriven_(false), 
  isTrackerDriven_(false),
  isolEmDR04_(-999.),
  isolHadDepth1DR04_(-999.),
  isolHadDepth2DR04_(-999.),
  isolPtTrksDR04_(-999.), 
  seedId_(0),
  isBarrel_(false),
  isEBEEGap_(false), 
  isEBEtaGap_(false),  
  isEBPhiGap_(false), 
  isEEDeeGap_(false),  
  isEERingGap_(false),
  posChargeTrk_(false),
  nrMissingHits_(-1),
  dCotTheta_(-1),
  dist_(-1),
  radius_(-999),
  isolChargedHadron_(-999.),
  isolNeutralHadron_(-999.),
  isolPhoton_(-999.),
  isolEcalCluster_(-999.),
  isolHcalCluster_(-999.),
  hademDepth1BC_(-999.),
  hademDepth2BC_(-999.),
  isolHadDepth1BC_(-999.),
  isolHadDepth2BC_(-999.),
  dxyErr_(-999.),
  dzErr_(-999.),
  isolMVA_(-999.),
  nonIsolMVA_(-999.),
  passCutPreSel_(false),
  passMVAPreSel_(false),
  passPFlowPreSel_(false),  
  pmDPhi1_(-999.),
  pmDPhi2_(-999.),
  pmDRz1_(-999.),
  pmDRz2_(-999.),
  pmSubDets_(0),
  nrSatCrysIn5x5_(-1),
  chargeInfo_(0),
  rhoCorr_(-999.),
  mEvent_(NULL)
{

}

SHElectron::SHElectron(const SHElectron &rhs):TObject(rhs),
  //classification variables
  type_(rhs.type_),
//kinematic quantities
  p4_(rhs.p4_),
  et_(rhs.et_),
  nrgy_(rhs.nrgy_),
  nrgyErr_(rhs.nrgyErr_),
  rawNrgy_(rhs.rawNrgy_),
  preShowerNrgy_(rhs.preShowerNrgy_),
  epCombNrgy_(rhs.epCombNrgy_),
  phoNrgy_(rhs.phoNrgy_),
  altNrgy_(rhs.altNrgy_),
  altNrgyErr_(rhs.altNrgyErr_),
  altEPCombNrgy_(rhs.altEPCombNrgy_),
  posCal_(rhs.posCal_),
  e5x5_(rhs.e5x5_),
  r9_(rhs.r9_),
  eta_(rhs.eta_),
  detEta_(rhs.detEta_),
//track quantities (momentum + positions)
//momemtums
  p3TrackVtx_(rhs.p3TrackVtx_),
  p3TrackCal_(rhs.p3TrackCal_),
  p3TrackInn_(rhs.p3TrackInn_),
  p3TrackOut_(rhs.p3TrackOut_),
//positions
  posTrackVtx_(rhs.posTrackVtx_),
  posTrackCal_(rhs.posTrackCal_),
  posTrackInn_(rhs.posTrackInn_),
  posTrackOut_(rhs.posTrackOut_),
  trkChi2_(rhs.trkChi2_),
  nrDof_(rhs.nrDof_),
  posCharge_(rhs.posCharge_),
  d0_(rhs.d0_),
  trkMomErr_(rhs.trkMomErr_),
//id quantities
  epIn_(rhs.epIn_),
  epOut_(rhs.epOut_),
  hadem_(rhs.hadem_),
  hademDepth1_(rhs.hademDepth1_),
  hademDepth2_(rhs.hademDepth2_),
  dEtaIn_(rhs.dEtaIn_),
  dEtaOut_(rhs.dEtaOut_),
  dPhiIn_(rhs.dPhiIn_),
  dPhiOut_(rhs.dPhiOut_),
  sigmaEtaEta_(rhs.sigmaEtaEta_),
  sigmaIEtaIEta_(rhs.sigmaIEtaIEta_),
  sigmaIPhiIPhi_(rhs.sigmaIPhiIPhi_),
  //links to tracks, superClusters
  superClusIndx_(-1), //we do not copy the index as its likely no longer correct
  //new isolation variables
  isolEm_(rhs.isolEm_),
  isolHad_(rhs.isolHad_),
  isolHadDepth1_(rhs.isolHadDepth1_),
  isolHadDepth2_(rhs.isolHadDepth2_),
  isolPtTrks_(rhs.isolPtTrks_),
  isolNrTrks_(rhs.isolNrTrks_),
  cutCode_(rhs.cutCode_),
  cutCodeVID_(rhs.cutCodeVID_),
  e1x5Over5x5_(rhs.e1x5Over5x5_),
  e2x5Over5x5_(rhs.e2x5Over5x5_),
  isEcalDriven_(rhs.isEcalDriven_), 
  isTrackerDriven_(rhs.isTrackerDriven_),
  isolEmDR04_(rhs.isolEmDR04_),
  isolHadDepth1DR04_(rhs.isolHadDepth1DR04_),
  isolHadDepth2DR04_(rhs.isolHadDepth2DR04_),
  isolPtTrksDR04_(rhs.isolPtTrksDR04_), 
  seedId_(rhs.seedId_),
  isBarrel_(rhs.isBarrel_),
  isEBEEGap_(rhs.isEBEEGap_), 
  isEBEtaGap_(rhs.isEBEtaGap_),  
  isEBPhiGap_(rhs.isEBPhiGap_), 
  isEEDeeGap_(rhs.isEEDeeGap_),  
  isEERingGap_(rhs.isEERingGap_),
  posChargeTrk_(rhs.posChargeTrk_),
  posTrackInnToSeed_(rhs.posTrackInnToSeed_),
  posTrackOutToSeed_(rhs.posTrackOutToSeed_),
  nrMissingHits_(rhs.nrMissingHits_),
  dCotTheta_(rhs.dCotTheta_),
  dist_(rhs.dist_),
  radius_(rhs.radius_), 
  isolChargedHadron_(rhs.isolChargedHadron_),
  isolNeutralHadron_(rhs.isolNeutralHadron_),
  isolPhoton_(rhs.isolPhoton_),  
  isolEcalCluster_(rhs.isolEcalCluster_),
  isolHcalCluster_(rhs.isolHcalCluster_),
  hademDepth1BC_(rhs.hademDepth1BC_),
  hademDepth2BC_(rhs.hademDepth2BC_),
  isolHadDepth1BC_(rhs.isolHadDepth1BC_),
  isolHadDepth2BC_(rhs.isolHadDepth2BC_),
  dxyErr_(rhs.dxyErr_),
  dzErr_(rhs.dzErr_),
  isolMVA_(rhs.isolMVA_),
  nonIsolMVA_(rhs.nonIsolMVA_), 
  passCutPreSel_(rhs.passCutPreSel_),
  passMVAPreSel_(rhs.passMVAPreSel_),
  passPFlowPreSel_(rhs.passPFlowPreSel_),  
  pmDPhi1_(rhs.pmDPhi1_),
  pmDPhi2_(rhs.pmDPhi2_),
  pmDRz1_(rhs.pmDRz1_),
  pmDRz2_(rhs.pmDRz2_),
  pmSubDets_(rhs.pmSubDets_),
  nrSatCrysIn5x5_(rhs.nrSatCrysIn5x5_),
  userFloats_(rhs.userFloats_),
  userInts_(rhs.userInts_),
  userIDs_(rhs.userIDs_),
  chargeInfo_(rhs.chargeInfo_),
  rhoCorr_(rhs.rhoCorr_),
  mEvent_(NULL)//dito for mEvent, its unlikely to be correct anymore
{

}

const SHSuperCluster* SHElectron::superClus()const
{
  if(mEvent_==NULL ||  superClusIndx_>=mEvent_->nrSuperClus()) return NULL; //cant get super clus as have no mother event
  else return mEvent_->getSuperClus(superClusIndx_);

}

const SHBasicCluster* SHElectron::seedClus()const
{
  if(superClus()==NULL || superClus()->nrClus()<=0) return NULL; //cant get super clus as have no mother event
  else return superClus()->seedClus();
}

int SHElectron::seedIdFromClus()const
{
  const SHBasicCluster* seed = seedClus();
  if(seed) return seed->seedId();
  else return 0;
}


float SHElectron::et()const
{
  return p4().Pt();

}


float SHElectron::dxy()const
{
  return mEvent_!=NULL ? dxy(mEvent_->vertex()) : dxy00();
}

float SHElectron::dxy(const TVector3& point)const
{
  const TVector3& trkPoint = posTrackInn();
  const TVector3& trkMom = p3TrackInn();
  return ( - (trkPoint.X()-point.X()) * trkMom.Y() + (trkPoint.Y()-point.Y()) * trkMom.X() ) / trkMom.Pt(); 
}

float SHElectron::dzTrkVtx()const
{
  const TVector3& pos = posTrackVtx();
  const TVector3& p3 = p3TrackVtx();
  return pos.Z()-(pos.X()*p3.X()+pos.Y()*p3.Y())/p3.Pt() * p3.Z()/p3.Pt();
}

std::pair<int,float> SHElectron::isolTrk(double minDeltaR,double maxDeltaR,double lipCut,double ptCut)const
{
  if(mEvent_==NULL) return std::pair<int,float>(999999,999999.); //need event link
  
  int nrTracks=0;
  float ptSum=0.;

  for(int trkNr=0;trkNr<mEvent_->nrIsolTrks();trkNr++){
    const SHIsolTrack* trk = mEvent_->getIsolTrk(trkNr);
    double deltaR = trk->p3().DeltaR(p3TrackVtx());
    if(trk->pt()> ptCut &&
       fabs(trk->dz()-dzTrkVtx()) < lipCut &&
       fabs(deltaR) > minDeltaR && fabs(deltaR)<maxDeltaR){
      ptSum+=trk->pt();
      nrTracks++;
      // std::cout <<"track pt "<<trk.pt()<<" deltaR "<<deltaR<<" lip "<<trk.dz()-dzTrkVtx()<<std::endl;
    }
  }
  return std::pair<int,float>(nrTracks,ptSum);
  
}

int SHElectron::calPhiRoad()const
{
  float sinTheta = et()/nrgy();
  float et5x5 = e5x5()*sinTheta;
  if(et5x5<5) return 17;
  else if(et5x5<10) return 14;
  else if(et5x5<15) return 12;
  else if(et5x5<25) return 11;
  else if(et5x5<35) return 10;
  else if(et5x5<45) return 9;
  else if(et5x5<70) return 8;
  else if(et5x5<110) return 7;
  else if(et5x5<145) return 6;
  else if(et5x5<205) return 5;
  else if(et5x5<270) return 3;
  else return 2;
}

void SHElectron::setNewNrgy(float newNrgy)
{
  float oldNrgy = nrgy_;

  p4_*=newNrgy/oldNrgy;
  nrgy_=newNrgy;
  nrgyErr_*=newNrgy/oldNrgy;
  et_*=newNrgy/oldNrgy;
  epIn_*=newNrgy/oldNrgy;
  epOut_*=newNrgy/oldNrgy;

  hadem_*=oldNrgy/newNrgy;
  hademDepth1_*=oldNrgy/newNrgy;
  hademDepth2_*=oldNrgy/newNrgy;

}
