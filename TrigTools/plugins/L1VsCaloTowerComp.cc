
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "SimDataFormats/SLHC/interface/L1CaloTowerFwd.h"
#include "SimDataFormats/SLHC/interface/L1CaloTriggerSetup.h"
#include "SimDataFormats/SLHC/interface/L1CaloTriggerSetupRcd.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerFwd.h"

#include "SHarper/TrigTools/interface/TrigToolsStructs.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"


#include "TTree.h"

class L1VsCaloTowerComp : public edm::EDAnalyzer {

public:
  struct TowerStruct {
    float emNrgy,hadNrgy,iEta,iPhi;
    static std::string contents(){return "emNrgy/F:hadNrgy:iEta:iPhi";}
    void fill(const l1slhc::L1CaloTower& tower);
    void fill(const CaloTower& tower);
    void clear(){emNrgy=hadNrgy=iEta=iPhi=-999;}
  };

private:
  edm::InputTag l1TowersTag_;
  edm::InputTag caloTowersTag_;
  
  edm::ESHandle <L1CaloTriggerSetup>  caloTriggerSetup_;


  TTree* tree_;
  TowerStruct recoTower_;
  TowerStruct l1Tower_;
  trigtools::EvtInfoStruct evtInfo_;

public:
  explicit L1VsCaloTowerComp(const edm::ParameterSet&);
  ~L1VsCaloTowerComp(){}
  
  virtual void beginJob();
  virtual void analyze(const edm::Event& ,const edm::EventSetup& );
  virtual void endJob(){}
  
  

};

L1VsCaloTowerComp::L1VsCaloTowerComp(const edm::ParameterSet& config)
{
  l1TowersTag_=config.getParameter<edm::InputTag>("l1TowersTag");
  caloTowersTag_=config.getParameter<edm::InputTag>("caloTowersTag");
}

void L1VsCaloTowerComp::beginJob()
{ 

  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_ = new TTree("ecalTowerTree","tree");
  tree_->Branch("evt",&evtInfo_,trigtools::EvtInfoStruct::contents().c_str());
  tree_->Branch("reco",&recoTower_,TowerStruct::contents().c_str());
  tree_->Branch("l1",&l1Tower_,TowerStruct::contents().c_str());
  

}

void L1VsCaloTowerComp::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup )
{
  edm::Handle<l1slhc::L1CaloTowerCollection> l1TowersHandle;
  iEvent.getByLabel(l1TowersTag_,l1TowersHandle);
  
  edm::Handle<CaloTowerCollection> caloTowersHandle;
  iEvent.getByLabel(caloTowersTag_,caloTowersHandle);
  
  iSetup.get<L1CaloTriggerSetupRcd>().get(caloTriggerSetup_);

  evtInfo_.fill(iEvent);

  std::vector<EtaPhiContainer<l1slhc::L1CaloTower>::const_iterator> usedTowers;
  // std::cout <<" number L1 towers "<<l1TowersHandle->size()<<std::endl;
  if(caloTowersHandle.isValid()){
    const edm::SortedCollection<CaloTower>& caloTowers = *caloTowersHandle;
    for(size_t caloTowerNr=0;caloTowerNr<caloTowers.size();caloTowerNr++){
      const CaloTower& tower = caloTowers[caloTowerNr];
      recoTower_.fill(tower);
      
      EtaPhiContainer<l1slhc::L1CaloTower>::const_iterator it = l1TowersHandle->find(tower.ieta(),tower.iphi());
      if(it!=l1TowersHandle->end()){
	usedTowers.push_back(it);
	//   std::cout <<"L1 E "<<it->E()<<" H "<<it->H()<<" eta "<<it->iEta()<<" phi "<<it->iPhi()<<" tower E "<<tower.emEnergy()<<" H "<<tower.hadEnergy()<<" eta "<<tower.ieta()<<" phi "<<tower.iphi()<<std::endl;
	//std::cout <<"it "<<(&(*it))<<" "<<&(*(l1TowersHandle->end()))<<std::endl;
	l1Tower_.fill(*it);
      }else l1Tower_.clear();
      tree_->Fill();
    }
  }
  std::sort(usedTowers.begin(),usedTowers.end());
  // std::cout <<"dump of L1 towers "<<std::endl;
  
  recoTower_.clear();
  typedef EtaPhiContainer<l1slhc::L1CaloTower>::const_iterator ConstIt;
  for(ConstIt it=l1TowersHandle->begin();it!=l1TowersHandle->end();++it){
    if(!std::binary_search(usedTowers.begin(),usedTowers.end(),it)){
      l1Tower_.fill(*it);
      tree_->Fill();
    }
    //   std::cout <<"L1 E "<<it->E()<<" H "<<it->H()<<" eta "<<it->iEta()<<" phi "<<it->iPhi()<<std::endl;

  }
  

}

void L1VsCaloTowerComp::TowerStruct::fill(const l1slhc::L1CaloTower& tower)
{
  emNrgy = tower.E();
  hadNrgy = tower.H();
  iEta = tower.iEta();
  iPhi = tower.iPhi();
}

void L1VsCaloTowerComp::TowerStruct::fill(const CaloTower& tower)
{
  emNrgy = tower.emEnergy();
  hadNrgy = tower.hadEnergy();
  iEta = tower.ieta();
  iPhi = tower.iphi();
}

			 

DEFINE_FWK_MODULE(L1VsCaloTowerComp);
