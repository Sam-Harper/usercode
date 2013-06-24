#include "SHarper/SHNtupliser/interface/SHL1Ntupliser.h"

#include "SHarper/SHNtupliser/interface/SHL1Event.hh"

#include "SimDataFormats/SLHC/interface/L1CaloClusterFwd.h"
#include "SimDataFormats/SLHC/interface/L1CaloCluster.h"

#include "FWCore/Framework/interface/MakerMacros.h"

void SHL1Ntupliser::initSHEvent()
{
  if(shEvt_) delete shEvt_;
  shL1Evt_ = new SHL1Event;
  shEvt_=shL1Evt_;
}

SHL1Ntupliser::SHL1Ntupliser(const edm::ParameterSet& iPara):SHNtupliser(iPara)

{
  l1CaloClustersTag_ = iPara.getParameter<edm::InputTag>("l1CaloClustersTag");
  l1CaloTowersTag_ = iPara.getParameter<edm::InputTag>("l1CaloTowersTag");
 
}

SHL1Ntupliser::~SHL1Ntupliser()
{
  
}


void SHL1Ntupliser::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  fillSHEvent(iEvent,iSetup);

  edm::Handle<l1slhc::L1CaloClusterCollection> l1CaloClusters;
  iEvent.getByLabel(l1CaloClustersTag_,l1CaloClusters);

  edm::Handle<l1slhc::L1CaloTowerCollection> l1CaloTowers;
  iEvent.getByLabel(l1CaloTowersTag_,l1CaloTowers);
  
  
  typedef l1slhc::L1CaloClusterCollection::const_iterator ConstClusIt;
  for(ConstClusIt clusIt = l1CaloClusters->begin();clusIt!=l1CaloClusters->end();++clusIt){
    shL1Evt_->addL1Clus(SHL1Cluster(*clusIt));
  }

  SHL1CaloTowerContainer towers;
  typedef l1slhc::L1CaloTowerCollection::const_iterator ConstTowerIt;
  for(ConstTowerIt towerIt = l1CaloTowers->begin();towerIt!=l1CaloTowers->end();++towerIt){
    towers.addTower(SHL1CaloTower(towerIt->iEta(),towerIt->iPhi(),
				  towerIt->E(),towerIt->H(),
				  towerIt->EcalFG(),towerIt->HcalFG()));
  }
  shL1Evt_->addL1CaloTowers(towers);
  

  fillTree();
}





//define this as a plug-in
DEFINE_FWK_MODULE(SHL1Ntupliser);
