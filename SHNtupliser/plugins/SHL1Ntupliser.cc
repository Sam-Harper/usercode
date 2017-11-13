#include "SHarper/SHNtupliser/interface/SHL1Ntupliser.h"

#include "SHarper/SHNtupliser/interface/SHL1Event.hh"


const l1t::CaloCluster* getEgammaCaloCluster(const l1t::EGamma& egamma,edm::Handle<l1t::CaloClusterBxCollection> l1CaloClusters);


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
  l1CaloClustersToken_ = consumes<l1t::CaloClusterBxCollection>( iPara.getParameter<edm::InputTag>("l1CaloClusters") );
  l1CaloTowersToken_ = consumes<l1t::CaloTowerBxCollection>( iPara.getParameter<edm::InputTag>("l1CaloTowers") );
  l1EgammasToken_ = consumes<l1t::EGammaBxCollection>( iPara.getParameter<edm::InputTag>("l1Egammas") );
 
}

SHL1Ntupliser::~SHL1Ntupliser()
{
  
}


void SHL1Ntupliser::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  fillSHEvent(iEvent,iSetup);
 
  edm::Handle<l1t::EGammaBxCollection> l1Egammas;
  iEvent.getByToken(l1EgammasToken_,l1Egammas);

  edm::Handle<l1t::CaloClusterBxCollection> l1CaloClusters;
  iEvent.getByToken(l1CaloClustersToken_,l1CaloClusters);

  edm::Handle<l1t::CaloTowerBxCollection> l1CaloTowers;
  iEvent.getByToken(l1CaloTowersToken_,l1CaloTowers);

  if(l1Egammas.isValid()){
    for(auto egammaIt = l1Egammas->begin(0);egammaIt!=l1Egammas->end(0);++egammaIt){
      const l1t::CaloCluster* caloClus = getEgammaCaloCluster(*egammaIt,l1CaloClusters);
      if(caloClus) shL1Evt_->addL1Clus(SHL1Cluster(*egammaIt,*caloClus));
      else shL1Evt_->addL1Clus(SHL1Cluster(*egammaIt));
    }
  }
  
  SHL1CaloTowerContainer towers;
  if(l1CaloTowers.isValid()){
    typedef l1t::CaloTowerBxCollection::const_iterator ConstTowerIt;
    for(ConstTowerIt towerIt = l1CaloTowers->begin(0);towerIt!=l1CaloTowers->end(0);++towerIt){

      int fgVeto = (towerIt->hwQual() & (0x1<<2)) == 0x4;
      //   if(fgVeto) std::cout <<"tower "<<towerIt->hwEta()<<" "<<towerIt->hwPhi()<<" "<<towerIt->hwEtEm()<<" "<<towerIt->hwEtHad()<<" fgVeto "<<fgVeto<<std::endl;
      if(towerIt->hwEtEm()+towerIt->hwEtHad()>0) towers.addTower(SHL1CaloTower(towerIt->hwEta(),towerIt->hwPhi(),
									       towerIt->hwEtEm(),towerIt->hwEtHad(),
									       fgVeto,towerIt->hwQual()));
    }
  }
  shL1Evt_->addL1CaloTowers(towers);
  
  // edm::InputTag l1UCT2015L1EGIsoTag("stage1L1extraParticles","Isolated");
  // edm::InputTag l1UCT2015L1EGNonIsoTag("stage1L1extraParticles","NonIsolated");
  // //edm::InputTag l1UCT2015L1EGRelaxedTag("uct2015L1ExtraParticles","Relaxed"); 

  // addL1Particles(l1UCT2015L1EGIsoTag,"caloStage1Iso",iEvent,shL1Evt_);
  // addL1Particles(l1UCT2015L1EGNonIsoTag,"caloStage1NonIso",iEvent,shL1Evt_);
  // //addL1Particles(l1UCT2015L1EGRelaxedTag,"l1UTC2015Relaxed",iEvent,shL1Evt_);

  fillTree();
}

const l1t::CaloCluster* getEgammaCaloCluster(const l1t::EGamma& egamma,edm::Handle<l1t::CaloClusterBxCollection> l1CaloClusters)
{
  if(l1CaloClusters.isValid()){
    for(auto clusIt = l1CaloClusters->begin(0);clusIt!=l1CaloClusters->end(0);++clusIt){
      if(egamma.hwEta()==clusIt->hwEta() && egamma.hwPhi()==clusIt->hwPhi() && egamma.hwPt()==clusIt->hwPt()){
	return &(*clusIt);
      }
    }
  }
  return 0;
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
    shEvent->addTrigInfo(trigInfo);
  }
}
  


//define this as a plug-in
DEFINE_FWK_MODULE(SHL1Ntupliser);
