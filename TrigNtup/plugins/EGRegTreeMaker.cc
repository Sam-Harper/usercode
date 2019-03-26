#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

#include "SHarper/TrigNtup/interface/EGRegTreeStruct.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>

class EGRegTreeMaker : public edm::EDAnalyzer {

private:
  EGRegTreeStruct egRegTreeData_;
  TTree* egRegTree_;

  edm::EDGetTokenT<reco::VertexCollection>  verticesToken_;
  edm::EDGetTokenT<double> rhoToken_;
  edm::EDGetTokenT<reco::GenParticleCollection> genPartsToken_;
  std::vector<edm::EDGetTokenT<reco::SuperClusterCollection>> scTokens_;
  edm::EDGetTokenT<EcalRecHitCollection> ecalHitsEBToken_;
  edm::EDGetTokenT<EcalRecHitCollection> ecalHitsEEToken_;
  edm::EDGetTokenT<std::vector<pat::Electron> > elesToken_;

  EGRegTreeMaker(const EGRegTreeMaker& rhs)=delete;
  EGRegTreeMaker& operator=(const EGRegTreeMaker& rhs)=delete;

public:
  explicit EGRegTreeMaker(const edm::ParameterSet& iPara);
  virtual ~EGRegTreeMaker();
  
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
  template<typename T>
  void setToken(std::vector<edm::EDGetTokenT<T> > & tokens,const edm::ParameterSet& iPara,const std::string& tagName){
    for(auto& tag: iPara.getParameter<std::vector<edm::InputTag> >(tagName)){
      tokens.push_back(consumes<T>(tag));
    }
  }
  static const reco::GenParticle* matchGenPart(float eta,float phi,const std::vector<reco::GenParticle>& genParts);
  static const reco::SuperCluster*  matchSC(float eta,float phi,const std::vector<edm::Handle<reco::SuperClusterCollection> >& scHandles);
};



EGRegTreeMaker::EGRegTreeMaker(const edm::ParameterSet& iPara):
  egRegTree_(nullptr)
{
  setToken(verticesToken_,iPara,"verticesTag");
  setToken(rhoToken_,iPara,"rhoTag");
  setToken(genPartsToken_,iPara,"genPartsTag");
  setToken(scTokens_,iPara,"scTag");
  setToken(ecalHitsEBToken_,iPara,"ecalHitsEBTag");
  setToken(ecalHitsEEToken_,iPara,"ecalHitsEETag");
  setToken(elesToken_,iPara,"elesTag");
}

EGRegTreeMaker::~EGRegTreeMaker()
{

}


void EGRegTreeMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  egRegTree_ = new TTree("egRegTree","");
  egRegTreeData_.createBranches(egRegTree_);
} 

void EGRegTreeMaker::beginRun(const edm::Run& run,const edm::EventSetup& iSetup)
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
  template<typename T> 
  std::vector<edm::Handle<T> > getHandle(const edm::Event& iEvent,const std::vector<edm::EDGetTokenT<T> >& tokens)
  {
    std::vector<edm::Handle<T> > handles;
    for(auto& token : tokens){
      edm::Handle<T> handle;
      iEvent.getByToken(token,handle);
      handles.emplace_back(std::move(handle));
    }
    return handles;
  }
}
  
namespace{

  bool hasBasicClusters(const reco::SuperCluster& sc){
    if(!sc.seed().isAvailable()) return false;
    else if(!sc.clusters().isAvailable()) return false;
    else return true;
  }
  const pat::Electron* matchEle(unsigned int seedId,const std::vector<pat::Electron>& eles){
    for(auto& ele : eles){
      if(ele.superCluster()->seed()->seed().rawId()==seedId) return &ele;
    }
    return nullptr;
  }
}

void EGRegTreeMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  auto scHandles = getHandle(iEvent,scTokens_);
  auto ecalHitsEBHandle = getHandle(iEvent,ecalHitsEBToken_);
  auto ecalHitsEEHandle = getHandle(iEvent,ecalHitsEEToken_);
  auto genPartsHandle = getHandle(iEvent,genPartsToken_);
  auto verticesHandle = getHandle(iEvent,verticesToken_);
  auto rhoHandle = getHandle(iEvent,rhoToken_);
  auto elesHandle = getHandle(iEvent,elesToken_);
  edm::ESHandle<CaloTopology> caloTopoHandle;
  iSetup.get<CaloTopologyRecord>().get(caloTopoHandle);

  int nrVert = verticesHandle->size();
  bool fillFromSC = false;
  if(fillFromSC){
    for(const auto& scHandle : scHandles){
      for(const auto& sc: *scHandle){
	const reco::GenParticle* genPart = genPartsHandle.isValid() ? matchGenPart(sc.eta(),sc.phi(),*genPartsHandle) : nullptr;
	const pat::Electron* ele = elesHandle.isValid() ? matchEle(sc.seed()->seed().rawId(),*elesHandle) : nullptr;
	
	if(hasBasicClusters(sc)){
	  egRegTreeData_.fill(iEvent,nrVert,*rhoHandle,
			      *ecalHitsEBHandle,*ecalHitsEEHandle,
			      *caloTopoHandle,
			      &sc,genPart,ele);
	  egRegTree_->Fill();
	}
      }
    }
  }else{

    for(const auto& genPart : *genPartsHandle){
      if(std::abs(genPart.pdgId())==11 && genPart.statusFlags().isPrompt() && genPart.statusFlags().isFirstCopy()){
	const reco::SuperCluster* sc = matchSC(genPart.eta(),genPart.phi(),scHandles);
	const pat::Electron* ele = elesHandle.isValid() && sc ? matchEle(sc->seed()->seed().rawId(),*elesHandle) : nullptr;
	

	egRegTreeData_.fill(iEvent,nrVert,*rhoHandle,
			    *ecalHitsEBHandle,*ecalHitsEEHandle,
			    *caloTopoHandle,
			    sc,&genPart,ele);
	egRegTree_->Fill();
      }
    }
    
  } 
}

const reco::GenParticle*  EGRegTreeMaker::matchGenPart(float eta,float phi,const std::vector<reco::GenParticle>& genParts)
{
  const reco::GenParticle* bestMatch=nullptr;
  float bestDR2=0.3*0.3;
  for(const auto& genPart : genParts){
    if(std::abs(genPart.pdgId())==11){
      if(genPart.statusFlags().isPrompt() && genPart.statusFlags().isFirstCopy()){
	float dR2 = reco::deltaR2(genPart.eta(),genPart.phi(),eta,phi);
	if(dR2<bestDR2){
	  bestMatch = &genPart;
	  bestDR2 = dR2;
	}
	  
      }
    }
  }
  return bestMatch;
}


const reco::SuperCluster*  EGRegTreeMaker::matchSC(float eta,float phi,const std::vector<edm::Handle<reco::SuperClusterCollection> >& scHandles)
{
  const reco::SuperCluster* bestMatch=nullptr;
  float bestDR2=0.3*0.3;
  for(const auto& scHandle : scHandles){
    for(const auto& sc: *scHandle){
      float dR2 = reco::deltaR2(sc.eta(),sc.phi(),eta,phi);
      if(dR2<bestDR2){
	bestMatch = &sc;
	bestDR2 = dR2;
      }
    }
  }
  return bestMatch;
}

void EGRegTreeMaker::endJob()
{ 

}


  


//define this as a plug-in
DEFINE_FWK_MODULE(EGRegTreeMaker);
