#include "SHarper/SHNtupliser/interface/SHElectron.hh"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"


#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"




SHElectron::SHElectron(const heep::Ele& ele,int superClusNr):
  //classification variables
type_(ele.gsfEle().classification()),
//kinematic quantities
p4_(ele.gsfEle().px(),ele.gsfEle().py(),ele.gsfEle().pz(),ele.gsfEle().energy()),
et_(p4_.Pt()),
nrgy_(p4_.E()),
rawNrgy_(ele.gsfEle().superCluster()->rawEnergy()),
preShowerNrgy_(ele.gsfEle().superCluster()->preshowerEnergy()),
nrgyErr_(ele.gsfEle().caloEnergyError()),
posCal_(ele.gsfEle().caloPosition().X(),ele.gsfEle().caloPosition().Y(),ele.gsfEle().caloPosition().Z()),
e5x5_(ele.e5x5()),
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
//id quantities
epIn_(ele.gsfEle().eSuperClusterOverP()),
epOut_(ele.gsfEle().eSeedClusterOverPout()),
hadem_(ele.gsfEle().hadronicOverEm()),
hademDepth1_(-999.), //not currently filling but will later
hademDepth2_(-999.),//not currently filling but will later
dEtaIn_(ele.gsfEle().deltaEtaSuperClusterTrackAtVtx()),
dEtaOut_(ele.gsfEle().deltaEtaSeedClusterTrackAtCalo()),
dPhiIn_(ele.gsfEle().deltaPhiSuperClusterTrackAtVtx()),
dPhiOut_(ele.gsfEle().deltaPhiSeedClusterTrackAtCalo()),
sigmaEtaEta_(ele.sigmaEtaEta()),
sigmaIEtaIEta_(ele.sigmaIEtaIEta()),
//links to tracks, superClusters
superClusIndx_(superClusNr),
 //new isolation variables (filled elsewhere)
isolEm_(ele.isolEm()),
isolHad_(ele.isolHad()),
isolHadDepth1_(ele.isolHadDepth1()),
isolHadDepth2_(ele.isolHadDepth2()),
isolPtTrks_(ele.isolPtTrks()),
isolNrTrks_(ele.isolNrTrks()),
cutCode_(ele.cutCode()),
e1x5Over5x5_(ele.e1x5Over5x5()),
e2x5Over5x5_(ele.e2x5MaxOver5x5()),
mEvent_(NULL)
{
 
 

}
