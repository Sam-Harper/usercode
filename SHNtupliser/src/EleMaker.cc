#include "SHarper/SHNtupliser/interface/EleMaker.h"

#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaTowerIsolation.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaRecHitIsolation.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/PhotonTkIsolation.h"
#include "RecoCaloTools/MetaCollections/interface/CaloRecHitMetaCollections.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
void EleMaker::setup(const edm::ParameterSet& iConfig)
{
   
  intRadiusTk_=iConfig.getParameter<double>("intRadiusTk");
  ptMinTk_=iConfig.getParameter<double>("ptMinTk");
  maxVtxDistTk_=iConfig.getParameter<double>("maxVtxDistTk");
  maxDrbTk_=iConfig.getParameter<double>("maxDrbTk");
  etaSliceTk_=iConfig.getParameter<double>("etaSliceTk");
  intRadiusHcal_=iConfig.getParameter<double>("intRadiusHcal");
  etMinHcal_=iConfig.getParameter<double>("etMinHcal");
  intRadiusEcalBarrel_=iConfig.getParameter<double>("intRadiusEcalBarrel");
  intRadiusEcalEndcaps_=iConfig.getParameter<double>("intRadiusEcalEndcaps");
  jurassicWidth_=iConfig.getParameter<double>("jurassicWidth");
  etMinBarrel_=iConfig.getParameter<double>("etMinBarrel");
  eMinBarrel_=iConfig.getParameter<double>("eMinBarrel");
  etMinEndcaps_=iConfig.getParameter<double>("etMinEndcaps");
  eMinEndcaps_=iConfig.getParameter<double>("eMinEndcaps");
  vetoClustered_=iConfig.getParameter<bool>("vetoClustered");
  useNumCrystals_ =iConfig.getParameter<bool>("useNumCrystals");
}

cmssw::ShowerShape EleMaker::getShowerShape(const reco::SuperCluster& superClus,const heep::Event& heepEvent,bool isEB)const
{
  
  EgammaTowerIsolation had1(0.15,0,0,1,heepEvent.handles().caloTowers.product());
  EgammaTowerIsolation had2(0.15,0,0,2,heepEvent.handles().caloTowers.product());

  cmssw::ShowerShape showerShape;
  const CaloTopology * topology = heepEvent.handles().caloTopology.product();
  const CaloGeometry * geometry = heepEvent.handles().caloGeom.product();
  const reco::CaloCluster & seedCluster = *(superClus.seed()) ;
  const EcalRecHitCollection * recHits = 0 ;
  if (isEB) recHits = heepEvent.handles().ebRecHits.product();
  else recHits = heepEvent.handles().eeRecHits.product();
  std::vector<float> covariances = EcalClusterTools::covariances(seedCluster,recHits,topology,geometry) ;
  std::vector<float> localCovariances = EcalClusterTools::localCovariances(seedCluster,recHits,topology) ;
  showerShape.sigmaEtaEta = sqrt(covariances[0]) ;
  showerShape.sigmaIetaIeta = sqrt(localCovariances[0]) ;
  showerShape.e1x5 = EcalClusterTools::e1x5(seedCluster,recHits,topology)  ;
  showerShape.e2x5Max = EcalClusterTools::e2x5Max(seedCluster,recHits,topology)  ;
  showerShape.e5x5 = EcalClusterTools::e5x5(seedCluster,recHits,topology) ;
  showerShape.hcalDepth1OverEcal =had1.getTowerESum(&superClus)/superClus.energy();
  showerShape.hcalDepth2OverEcal =had2.getTowerESum(&superClus)/superClus.energy();
  showerShape.eMax = EcalClusterTools::eMax(seedCluster,recHits);
  showerShape.e3x3 = EcalClusterTools::e3x3(seedCluster,recHits,topology);
  return showerShape;
}




cmssw::IsolationVariables EleMaker::getIsol(const reco::SuperCluster& superClus,const heep::Event& heepEvent,float radius)const
{
  cmssw::IsolationVariables isol;
  
  EgammaTowerIsolation had1Iso(radius,intRadiusHcal_,etMinHcal_,1,heepEvent.handles().caloTowers.product());
  EgammaTowerIsolation had2Iso(radius,intRadiusHcal_,etMinHcal_,2,heepEvent.handles().caloTowers.product());
  EcalRecHitMetaCollection ecalBarrelHits(*heepEvent.ebHitsFull());
  EcalRecHitMetaCollection ecalEndcapHits(*heepEvent.eeHitsFull());
  EgammaRecHitIsolation ecalBarrelIsol(radius,intRadiusEcalBarrel_,jurassicWidth_,etMinBarrel_,eMinBarrel_,
				       heepEvent.handles().caloGeom.product(),&ecalBarrelHits,DetId::Ecal);
  EgammaRecHitIsolation ecalEndcapIsol(radius,intRadiusEcalEndcaps_,jurassicWidth_,etMinEndcaps_,eMinEndcaps_,
				       heepEvent.handles().caloGeom.product(),&ecalEndcapHits,DetId::Ecal);
  ecalBarrelIsol.setUseNumCrystals(useNumCrystals_);
  ecalBarrelIsol.setVetoClustered(vetoClustered_);
  ecalEndcapIsol.setUseNumCrystals(useNumCrystals_);
  ecalEndcapIsol.setVetoClustered(vetoClustered_);
 
  PhotonTkIsolation trackIsol(radius,intRadiusTk_,etaSliceTk_,ptMinTk_,maxVtxDistTk_,maxDrbTk_,heepEvent.handles().ctfTrack.product(),heepEvent.handles().beamSpot->position());
  
  math::XYZPoint vtx;
  if(heepEvent.handles().vertices->size()!=0) vtx=heepEvent.handles().vertices->front().position();
  //  isol.tkSumPt = trackIsol.getIso(superClus.position(),vtx).second;
  isol.hcalDepth1TowerSumEt = had1Iso.getTowerEtSum(&superClus);
  isol.hcalDepth2TowerSumEt = had2Iso.getTowerEtSum(&superClus);
  // isol.ecalRecHitSumEt = ecalBarrelIsol.getEtSum(&superClus)+ecalEndcapIsol.getEtSum(&superClus);


  return isol;
}

cmssw::FiducialFlags EleMaker::getFid(const reco::SuperCluster& superClus,const heep::Event& heepEvent)const
{
  DetId seedXtalId = superClus.seed()->hitsAndFractions()[0].first ;

  cmssw::FiducialFlags fiducialFlags ;
  int detector = seedXtalId.subdetId() ;
  double feta=fabs(superClus.position().eta()) ;
  if (detector==EcalBarrel){
    fiducialFlags.isEB = true ;
    EBDetId ebdetid(seedXtalId);
    if (EBDetId::isNextToEtaBoundary(ebdetid)){
      if (ebdetid.ietaAbs()==85) fiducialFlags.isEBEEGap = true ; 
      else fiducialFlags.isEBEtaGap = true ; 
    }
    if (EBDetId::isNextToPhiBoundary(ebdetid)) fiducialFlags.isEBPhiGap = true ; 
  }
  else if (detector==EcalEndcap){
    fiducialFlags.isEE = true ;
    EEDetId eedetid(seedXtalId);
    if (EEDetId::isNextToRingBoundary(eedetid)) {
      if (fabs(feta)<2.) fiducialFlags.isEBEEGap = true ; 
      else fiducialFlags.isEERingGap = true ; 
    }
    if (EEDetId::isNextToDBoundary(eedetid)) fiducialFlags.isEEDeeGap = true ; 
  }
  return fiducialFlags;
}

TLorentzVector EleMaker::getP4(const reco::SuperCluster& superClus,const heep::Event& heepEvent)const
{  
  math::XYZPoint vtx;
  if(heepEvent.handles().vertices->size()!=0) vtx=heepEvent.handles().vertices->front().position();
  math::XYZVector direction = superClus.position() - vtx;
  math::XYZVector mom = direction.unit()*superClus.energy(); 
  TLorentzVector p4(mom.x(),mom.y(),mom.z(),superClus.energy());
  return p4;
 

}
