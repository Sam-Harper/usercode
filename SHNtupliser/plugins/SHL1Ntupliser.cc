#include "SHarper/SHNtupliser/interface/SHL1Ntupliser.h"

#include "SHarper/SHNtupliser/interface/SHL1Event.hh"

//Will fix this really nasty hack with some proper branch setup once I've got this all sorted
#define SLHCBUILD 0
#if SLHCBUILD
#include "SimDataFormats/SLHC/interface/L1CaloClusterFwd.h"
#include "SimDataFormats/SLHC/interface/L1CaloCluster.h"
#endif

#include "FWCore/Framework/interface/MakerMacros.h"

//little hacky naughty temp function
void addL1Particles(const edm::InputTag& tag,const std::string& l1Name,const edm::Event& iEvent,SHEvent* shEvent);


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

  #if SLHCBUILD
  edm::Handle<l1slhc::L1CaloClusterCollection> l1CaloClusters;
  iEvent.getByLabel(l1CaloClustersTag_,l1CaloClusters);

  edm::Handle<l1slhc::L1CaloTowerCollection> l1CaloTowers;
  iEvent.getByLabel(l1CaloTowersTag_,l1CaloTowers);

  if(l1CaloClusters.isValid()){
    typedef l1slhc::L1CaloClusterCollection::const_iterator ConstClusIt;
    for(ConstClusIt clusIt = l1CaloClusters->begin();clusIt!=l1CaloClusters->end();++clusIt){
      shL1Evt_->addL1Clus(SHL1Cluster(*clusIt));
    }
  }
  
  SHL1CaloTowerContainer towers;
  if(l1CaloTowers.isValid()){
    typedef l1slhc::L1CaloTowerCollection::const_iterator ConstTowerIt;
    for(ConstTowerIt towerIt = l1CaloTowers->begin();towerIt!=l1CaloTowers->end();++towerIt){
      towers.addTower(SHL1CaloTower(towerIt->iEta(),towerIt->iPhi(),
				    towerIt->E(),towerIt->H(),
				    towerIt->EcalFG(),towerIt->HcalFG()));
    }
  }
  shL1Evt_->addL1CaloTowers(towers);
  #endif
  
  edm::InputTag l1UCT2015L1EGIsoTag("uct2015L1ExtraParticles","Isolated");
  edm::InputTag l1UCT2015L1EGNonIsoTag("uct2015L1ExtraParticles","NonIsolated");
  edm::InputTag l1UCT2015L1EGRelaxedTag("uct2015L1ExtraParticles","Relaxed"); 

  addL1Particles(l1UCT2015L1EGIsoTag,"l1UTC2015Iso",iEvent,shL1Evt_);
  addL1Particles(l1UCT2015L1EGNonIsoTag,"l1UTC2015NonIso",iEvent,shL1Evt_);
  addL1Particles(l1UCT2015L1EGRelaxedTag,"l1UTC2015Relaxed",iEvent,shL1Evt_);

  fillTree();
}

void addL1Particles(const edm::InputTag& tag,const std::string& l1Name,const edm::Event& iEvent,SHEvent* shEvent)
{
  edm::Handle<std::vector<l1extra::L1EmParticle> > egHandle;
  iEvent.getByLabel(tag,egHandle);
  if(egHandle.isValid()){
    const std::vector<l1extra::L1EmParticle>& egs = *egHandle;
    SHTrigInfo trigInfo(-1,l1Name);
    for(size_t egNr=0;egNr<egs.size();egNr++){
      TLorentzVector p4;
      p4.SetPtEtaPhiM(egs[egNr].pt(),egs[egNr].eta(),egs[egNr].phi(),egs[egNr].mass());
      trigInfo.addObj(p4);
    }
  }
}
  





//define this as a plug-in
DEFINE_FWK_MODULE(SHL1Ntupliser);
