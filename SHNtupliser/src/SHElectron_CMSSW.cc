#include "SHarper/SHNtupliser/interface/SHElectron.hh"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"


#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"




SHElectron::SHElectron(const heep::Ele& ele,int superClusNr):
  //classification variables
type_(ele.gsfEle().classification()),
//kinematic quantities
p4_(ele.p4().px(),ele.p4().py(),ele.p4().pz(),ele.energy()),
et_(p4_.Pt()),
nrgy_(ele.caloEnergy()),
rawNrgy_(ele.gsfEle().superCluster()->rawEnergy()),
preShowerNrgy_(ele.gsfEle().superCluster()->preshowerEnergy()),
nrgyErr_(ele.gsfEle().ecalEnergyError()),
posCal_(ele.gsfEle().caloPosition().X(),ele.gsfEle().caloPosition().Y(),ele.gsfEle().caloPosition().Z()),
e5x5_(ele.scE5x5()),
eta_(ele.eta()),
detEta_(ele.detEta()),
//track quantities (momentum + positions)
//momemtums
p3TrackVtx_(ele.gsfEle().trackMomentumAtVtx().X(),ele.gsfEle().trackMomentumAtVtx().Y(),ele.gsfEle().trackMomentumAtVtx().Z()),
p3TrackCal_(ele.gsfEle().trackMomentumAtCalo().X(),ele.gsfEle().trackMomentumAtCalo().Y(),ele.gsfEle().trackMomentumAtCalo().Z()),
p3TrackInn_(ele.gsfEle().gsfTrack()->innerMomentum().X(),ele.gsfEle().gsfTrack()->innerMomentum().Y(),ele.gsfEle().gsfTrack()->innerMomentum().Z()),
p3TrackOut_(ele.gsfEle().gsfTrack()->outerMomentum().X(),ele.gsfEle().gsfTrack()->outerMomentum().Y(),ele.gsfEle().gsfTrack()->outerMomentum().Z()),
//positions
posTrackVtx_(ele.gsfEle().TrackPositionAtVtx().X(),ele.gsfEle().TrackPositionAtVtx().Y(),ele.gsfEle().TrackPositionAtVtx().Z()),
posTrackCal_(ele.gsfEle().TrackPositionAtCalo().X(),ele.gsfEle().TrackPositionAtCalo().Y(),ele.gsfEle().TrackPositionAtCalo().Z()),
posTrackInn_(ele.gsfEle().gsfTrack()->innerPosition().X(),ele.gsfEle().gsfTrack()->innerPosition().Y(),ele.gsfEle().gsfTrack()->innerPosition().Z()),
posTrackOut_(ele.gsfEle().gsfTrack()->outerPosition().X(),ele.gsfEle().gsfTrack()->outerPosition().Y(),ele.gsfEle().gsfTrack()->outerPosition().Z()),
trkChi2_(ele.gsfEle().gsfTrack()->chi2()),
nrDof_(static_cast<int>(ele.gsfEle().gsfTrack()->ndof())),
posCharge_(ele.gsfEle().charge()>0),
d0_(999),
//id quantities
epIn_(ele.gsfEle().eSuperClusterOverP()),
epOut_(ele.gsfEle().eSeedClusterOverPout()),
hadem_(ele.gsfEle().hadronicOverEm()),
hademDepth1_(ele.gsfEle().hcalDepth1OverEcal()), 
hademDepth2_(ele.gsfEle().hcalDepth2OverEcal()),
dEtaIn_(ele.gsfEle().deltaEtaSuperClusterTrackAtVtx()),
dEtaOut_(ele.gsfEle().deltaEtaSeedClusterTrackAtCalo()),
dPhiIn_(ele.gsfEle().deltaPhiSuperClusterTrackAtVtx()),
dPhiOut_(ele.gsfEle().deltaPhiSeedClusterTrackAtCalo()),
sigmaEtaEta_(ele.scSigmaEtaEta()),
sigmaIEtaIEta_(ele.scSigmaIEtaIEta()),
//links to tracks, superClusters
superClusIndx_(superClusNr),
 //new isolation variables (filled elsewhere)
isolEm_(ele.isolEm()),
isolHad_(ele.isolHad()),
isolHadDepth1_(ele.isolHadDepth1()),
isolHadDepth2_(ele.isolHadDepth2()),
isolPtTrks_(ele.isolPtTrks()),
isolNrTrks_(-1), //no longer valid
cutCode_(ele.cutCode()),
e1x5Over5x5_(ele.scE1x5Over5x5()),
e2x5Over5x5_(ele.scE2x5MaxOver5x5()),
isEcalDriven_(ele.gsfEle().ecalDrivenSeed()),
isTrackerDriven_(ele.gsfEle().trackerDrivenSeed()),
isolEmDR04_(ele.gsfEle().dr04EcalRecHitSumEt()),
isolHadDepth1DR04_(ele.gsfEle().dr04HcalDepth1TowerSumEt()),
isolHadDepth2DR04_(ele.gsfEle().dr04HcalDepth2TowerSumEt()),
isolPtTrksDR04_(ele.gsfEle().dr04TkSumPt()),
epCombNrgy_(ele.gsfEle().energy()),
seedId_(ele.superCluster().seed()->seed().rawId()),
isBarrel_(ele.gsfEle().isEB()),
isEBEEGap_(ele.gsfEle().isEBEEGap()), 
isEBEtaGap_(ele.gsfEle().isEBEtaGap()),  
isEBPhiGap_(ele.gsfEle().isEBPhiGap()), 
isEEDeeGap_(ele.gsfEle().isEEDeeGap()),  
isEERingGap_(ele.gsfEle().isEERingGap()),
posChargeTrk_(ele.gsfEle().gsfTrack()->charge()),
mEvent_(NULL)
{
 
 

}

//fills off a GsfElectron, doesnt fill nr trks isol or cutcode
SHElectron::SHElectron(const reco::GsfElectron& ele,int superClusNr):
  //classification variables
type_(ele.classification()),
//kinematic quantities
p4_(ele.px()/ele.energy()*ele.ecalEnergy(),ele.py()/ele.energy()*ele.ecalEnergy(),ele.pz()/ele.energy()*ele.ecalEnergy(),ele.ecalEnergy()),
et_(p4_.Pt()),
nrgy_(ele.ecalEnergy()),
rawNrgy_(ele.superCluster()->rawEnergy()),
preShowerNrgy_(ele.superCluster()->preshowerEnergy()),
nrgyErr_(ele.ecalEnergyError()),
posCal_(ele.caloPosition().X(),ele.caloPosition().Y(),ele.caloPosition().Z()),
e5x5_(ele.e5x5()),
eta_(ele.eta()),
detEta_(ele.superCluster()->eta()),
//track quantities (momentum + positions)
//momemtums
p3TrackVtx_(ele.trackMomentumAtVtx().X(),ele.trackMomentumAtVtx().Y(),ele.trackMomentumAtVtx().Z()),
p3TrackCal_(ele.trackMomentumAtCalo().X(),ele.trackMomentumAtCalo().Y(),ele.trackMomentumAtCalo().Z()),
p3TrackInn_(ele.gsfTrack()->innerMomentum().X(),ele.gsfTrack()->innerMomentum().Y(),ele.gsfTrack()->innerMomentum().Z()),
p3TrackOut_(ele.gsfTrack()->outerMomentum().X(),ele.gsfTrack()->outerMomentum().Y(),ele.gsfTrack()->outerMomentum().Z()),
//positions
posTrackVtx_(ele.TrackPositionAtVtx().X(),ele.TrackPositionAtVtx().Y(),ele.TrackPositionAtVtx().Z()),
posTrackCal_(ele.TrackPositionAtCalo().X(),ele.TrackPositionAtCalo().Y(),ele.TrackPositionAtCalo().Z()),
posTrackInn_(ele.gsfTrack()->innerPosition().X(),ele.gsfTrack()->innerPosition().Y(),ele.gsfTrack()->innerPosition().Z()),
posTrackOut_(ele.gsfTrack()->outerPosition().X(),ele.gsfTrack()->outerPosition().Y(),ele.gsfTrack()->outerPosition().Z()),
trkChi2_(ele.gsfTrack()->chi2()),
nrDof_(static_cast<int>(ele.gsfTrack()->ndof())),
posCharge_(ele.charge()>0),
d0_(999),
//id quantities
epIn_(ele.eSuperClusterOverP()),
epOut_(ele.eSeedClusterOverPout()),
hadem_(ele.hcalOverEcal()),
hademDepth1_(ele.hcalDepth1OverEcal()), 
hademDepth2_(ele.hcalDepth2OverEcal()),
dEtaIn_(ele.deltaEtaSuperClusterTrackAtVtx()),
dEtaOut_(ele.deltaEtaSeedClusterTrackAtCalo()),
dPhiIn_(ele.deltaPhiSuperClusterTrackAtVtx()),
dPhiOut_(ele.deltaPhiSeedClusterTrackAtCalo()),
sigmaEtaEta_(ele.sigmaEtaEta()),
sigmaIEtaIEta_(ele.sigmaIetaIeta()),
//links to tracks, superClusters
superClusIndx_(superClusNr),
isolEm_(ele.dr03EcalRecHitSumEt()),
isolHad_(ele.dr03HcalDepth1TowerSumEt()),
isolHadDepth1_(ele.dr03HcalDepth1TowerSumEt()),
isolHadDepth2_(ele.dr03HcalDepth2TowerSumEt()),
isolPtTrks_(ele.dr03TkSumPt()),
isolNrTrks_(-1), //not really supported anymore
cutCode_(-1),
e1x5Over5x5_(ele.e1x5()/ele.e5x5()),
e2x5Over5x5_(ele.e2x5Max()/ele.e5x5()),
isEcalDriven_(ele.ecalDrivenSeed()),
isTrackerDriven_(ele.trackerDrivenSeed()),
isolEmDR04_(ele.dr04EcalRecHitSumEt()),
isolHadDepth1DR04_(ele.dr04HcalDepth1TowerSumEt()),
isolHadDepth2DR04_(ele.dr04HcalDepth2TowerSumEt()),
isolPtTrksDR04_(ele.dr04TkSumPt()),
epCombNrgy_(ele.energy()),
seedId_(ele.superCluster()->seed()->seed().rawId()),
isBarrel_(ele.isEB()),
isEBEEGap_(ele.isEBEEGap()), 
isEBEtaGap_(ele.isEBEtaGap()),  
isEBPhiGap_(ele.isEBPhiGap()), 
isEEDeeGap_(ele.isEEDeeGap()),  
isEERingGap_(ele.isEERingGap()),
posChargeTrk_(ele.gsfTrack()->charge()),
mEvent_(NULL)
{
 
 

}
//makes a trackless electron
SHElectron::SHElectron(const TLorentzVector&p4,const reco::SuperCluster& superClus,
		       const cmssw::FiducialFlags& fid,
		       const cmssw::ShowerShape& shape,
		       const cmssw::IsolationVariables& isol03,
		       const cmssw::IsolationVariables& isol04,
		       int superClusNr):
//classification variables
  type_(0), //all are golden (dump this variable...)
  //kinematic quantities
  p4_(p4),
  et_(p4_.Pt()),
  nrgy_(p4_.E()),
  rawNrgy_(superClus.rawEnergy()),
  preShowerNrgy_(superClus.preshowerEnergy()),
  nrgyErr_(-999), //too complicated to fill for now
  posCal_(superClus.x(),superClus.y(),superClus.z()),
  e5x5_(shape.e5x5),
  eta_(p4.Eta()),
  detEta_(superClus.eta()),
  //track quantities (momentum + positions)
  //momemtums
  p3TrackVtx_(0.001,0,0.),
  p3TrackCal_(0.001,0,0.),
  p3TrackInn_(0.001,0,0.),
  p3TrackOut_(0.001,0,0.),
  //positions
  posTrackVtx_(0.001,0,0.),
  posTrackCal_(0.001,0,0.),
  posTrackInn_(0.001,0,0.),
  posTrackOut_(0.001,0,0.),
  trkChi2_(999999),
  nrDof_(static_cast<int>(1)),
  posCharge_(0),
  d0_(999),
  //id quantities
  epIn_(999999),
  epOut_(999999),
  hadem_(shape.hcalDepth1OverEcal+shape.hcalDepth2OverEcal),
  hademDepth1_(shape.hcalDepth1OverEcal), 
  hademDepth2_(shape.hcalDepth2OverEcal),
  dEtaIn_(999),
  dEtaOut_(999),
  dPhiIn_(999),
  dPhiOut_(999),
  sigmaEtaEta_(shape.sigmaEtaEta),
  sigmaIEtaIEta_(shape.sigmaIetaIeta),
  //links to tracks, superClusters
  superClusIndx_(superClusNr),
  isolEm_(isol03.ecalRecHitSumEt),
  isolHad_(isol03.hcalDepth1TowerSumEt+isol03.hcalDepth2TowerSumEt),
  isolHadDepth1_(isol03.hcalDepth1TowerSumEt),
  isolHadDepth2_(isol03.hcalDepth2TowerSumEt),
  isolPtTrks_(isol03.tkSumPt),
  isolNrTrks_(-1), //not really supported anymore
  cutCode_(-1),
  e1x5Over5x5_(shape.e1x5/shape.e5x5),
  e2x5Over5x5_(shape.e2x5Max/shape.e5x5),
  isEcalDriven_(1),
  isTrackerDriven_(0),
  isolEmDR04_(isol04.ecalRecHitSumEt),
  isolHadDepth1DR04_(isol04.hcalDepth1TowerSumEt),
  isolHadDepth2DR04_(isol04.hcalDepth2TowerSumEt),
  isolPtTrksDR04_(isol04.tkSumPt),
  epCombNrgy_(-999),
  seedId_(superClus.seed()->seed().rawId()),
  isBarrel_(fid.isEB),
  isEBEEGap_(fid.isEBEEGap), 
  isEBEtaGap_(fid.isEBEtaGap),  
  isEBPhiGap_(fid.isEBPhiGap), 
  isEEDeeGap_(fid.isEEDeeGap),  
  isEERingGap_(fid.isEERingGap),
  posChargeTrk_(0),
mEvent_(NULL)
{
}
