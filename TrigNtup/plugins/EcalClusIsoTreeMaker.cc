#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
 
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "RecoEgamma/EgammaIsolationAlgos/interface/EcalPFClusterIsolation.h"



#include "TFile.h"
#include "TTree.h"

#include <string>

struct EvtStruct {
  int runnr,lumiSec,eventnr;
  static std::string contents(){return "runnr/I:lumiSec:eventnr";}
  void clear(){runnr=lumiSec=eventnr=0;}
  void fill(const edm::Event& event);
};


struct PFEcalClusTreeStruct {
  EvtStruct evt;
  unsigned int seedId;
  float eleEt;
  float eleEta;
  float elePhi;
  float oldPFEcalIso;
  float newPFEcalIso;

  void createBranches(TTree* tree);
  void setBranchAddresses(TTree* tree){}
  void fill(const edm::Event& event,const edm::Ref<std::vector<reco::GsfElectron> > ele,const edm::ValueMap<float>& oldPfEcalIso,const edm::Handle<std::vector<reco::PFCluster> > newPFClusters);

};

class EcalClusIsoTreeMaker : public edm::one::EDAnalyzer<> {

private:
  PFEcalClusTreeStruct pfEcalClusData_;
  TTree* pfEcalClusTree_;


  edm::EDGetTokenT<edm::ValueMap<float> > pfEcalIsoToken_;
  edm::EDGetTokenT<std::vector<reco::PFCluster> > pfEcalClusToken_;
  edm::EDGetTokenT<std::vector<reco::GsfElectron> > elesToken_;

  EcalClusIsoTreeMaker(const EcalClusIsoTreeMaker& rhs)=delete;
  EcalClusIsoTreeMaker& operator=(const EcalClusIsoTreeMaker& rhs)=delete;

public:
  explicit EcalClusIsoTreeMaker(const edm::ParameterSet& iPara);
  virtual ~EcalClusIsoTreeMaker();
  
private:
  virtual void beginJob();
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endRun(edm::Run const& iRun, edm::EventSetup const&){}
  virtual void endJob();
  template<typename T>
  void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tagName){
    token = consumes<T>(iPara.getParameter<edm::InputTag>(tagName));
  }
};



EcalClusIsoTreeMaker::EcalClusIsoTreeMaker(const edm::ParameterSet& iPara):
  pfEcalClusTree_(nullptr)
{
  setToken(pfEcalIsoToken_,iPara,"pfEcalIsoTag");
  setToken(pfEcalClusToken_,iPara,"pfEcalClusTag");
  setToken(elesToken_,iPara,"elesTag");
}

EcalClusIsoTreeMaker::~EcalClusIsoTreeMaker()
{

}


void EcalClusIsoTreeMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  pfEcalClusTree_ = new TTree("pfEcalClusTree","");
  pfEcalClusData_.createBranches(pfEcalClusTree_);
} 

void EcalClusIsoTreeMaker::beginRun(const edm::Run& run,const edm::EventSetup& iSetup)
{ 
 
}

namespace {
  template<typename T> 
  edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token)
  {
    edm::Handle<T> handle;
    iEvent.getByToken(token,handle);
    return handle;
  }
}


void EcalClusIsoTreeMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  auto pfEcalIsoHandle = getHandle(iEvent,pfEcalIsoToken_);
  auto pfEcalClusHandle = getHandle(iEvent,pfEcalClusToken_);
  auto elesHandle = getHandle(iEvent,elesToken_);

  for(size_t eleNr = 0; eleNr<elesHandle->size(); eleNr++){
    edm::Ref<std::vector<reco::GsfElectron> > ele(elesHandle,eleNr);
    pfEcalClusData_.fill(iEvent,ele,*pfEcalIsoHandle,pfEcalClusHandle);
    pfEcalClusTree_->Fill();
  }
} 


void EcalClusIsoTreeMaker::endJob()
{ 

}

void PFEcalClusTreeStruct::createBranches(TTree* tree)
{
  tree->Branch("evt",&evt,evt.contents().c_str());
  tree->Branch("seedId",&seedId,"seedId/i");
  tree->Branch("eleEt",&eleEt,"eleEt/F");
  tree->Branch("eleEta",&eleEta,"eleEta/F");
  tree->Branch("elePhi",&elePhi,"elePhi/F");
  tree->Branch("oldPFEcalIso",&oldPFEcalIso,"oldPFEcalIso/F");
  tree->Branch("newPFEcalIso",&newPFEcalIso,"newPFEcalIso/F");
 
}


 
void PFEcalClusTreeStruct::fill(const edm::Event& event,edm::Ref<std::vector<reco::GsfElectron> > ele,const edm::ValueMap<float>& oldPfEcalIso,const edm::Handle<std::vector<reco::PFCluster> > newPFClusters)
{
  EcalPFClusterIsolation<reco::GsfElectron> pfClusIsoAlgo(0.3,0.,0.,0.,0.,0.,0.);
  
  evt.fill(event);
  seedId = ele->superCluster()->seed()->seed().rawId();
  eleEt = ele->et();
  eleEta = ele->eta();
  elePhi = ele->phi();
  oldPFEcalIso = oldPfEcalIso[ele];
  newPFEcalIso = pfClusIsoAlgo.getSum(ele,newPFClusters);
}
  


//define this as a plug-in
DEFINE_FWK_MODULE(EcalClusIsoTreeMaker);
  

