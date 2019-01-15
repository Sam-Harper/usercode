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

#include "SHarper/TrigNtup/interface/SCRegTreeStruct.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>

class SCRegTreeMaker : public edm::EDAnalyzer {

private:
  SCRegTreeStruct scRegTreeData_;
  TTree* scRegTree_;

  edm::EDGetTokenT<reco::VertexCollection>  verticesToken_;
  edm::EDGetTokenT<reco::GenParticleCollection> genPartsToken_;
  edm::EDGetTokenT<reco::SuperClusterCollection> scToken_;
  edm::EDGetTokenT<EcalRecHitCollection> ecalHitsEBToken_;
  edm::EDGetTokenT<EcalRecHitCollection> ecalHitsEEToken_;

  SCRegTreeMaker(const SCRegTreeMaker& rhs)=delete;
  SCRegTreeMaker& operator=(const SCRegTreeMaker& rhs)=delete;

public:
  explicit SCRegTreeMaker(const edm::ParameterSet& iPara);
  virtual ~SCRegTreeMaker();
  
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
  static const reco::GenParticle* matchGenPart(float eta,float phi,const std::vector<reco::GenParticle>& genParts);
};



SCRegTreeMaker::SCRegTreeMaker(const edm::ParameterSet& iPara):
  scRegTree_(nullptr)
{
  setToken(verticesToken_,iPara,"verticesTag");
  setToken(genPartsToken_,iPara,"genPartsTag");
  setToken(scToken_,iPara,"scTag");
  setToken(ecalHitsEBToken_,iPara,"ecalHitsEBTag");
  setToken(ecalHitsEEToken_,iPara,"ecalHitsEETag");
}

SCRegTreeMaker::~SCRegTreeMaker()
{

}


void SCRegTreeMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  scRegTree_ = new TTree("scRegTree","");
  scRegTreeData_.createBranches(scRegTree_);
} 

void SCRegTreeMaker::beginRun(const edm::Run& run,const edm::EventSetup& iSetup)
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
  
namespace{

  bool hasBasicClusters(const reco::SuperCluster& sc){
    if(!sc.seed().isAvailable()) return false;
    else if(!sc.clusters().isAvailable()) return false;
    else return true;
  }
}

void SCRegTreeMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  auto scHandle = getHandle(iEvent,scToken_);
  auto ecalHitsEBHandle = getHandle(iEvent,ecalHitsEBToken_);
  auto ecalHitsEEHandle = getHandle(iEvent,ecalHitsEEToken_);
  auto genPartsHandle = getHandle(iEvent,genPartsToken_);
  auto verticesHandle = getHandle(iEvent,verticesToken_);
  edm::ESHandle<CaloTopology> caloTopoHandle;
  iSetup.get<CaloTopologyRecord>().get(caloTopoHandle);

  int nrVert = verticesHandle->size();
  for(const auto& sc: *scHandle){
    const reco::GenParticle* genPart = genPartsHandle.isValid() ? matchGenPart(sc.eta(),sc.phi(),*genPartsHandle) : nullptr;
    
    if(hasBasicClusters(sc)){
      
    
      scRegTreeData_.fill(iEvent,nrVert,sc,*ecalHitsEBHandle,*ecalHitsEEHandle,*caloTopoHandle,genPart);
      scRegTree_->Fill();
    }
  }

} 

const reco::GenParticle*  SCRegTreeMaker::matchGenPart(float eta,float phi,const std::vector<reco::GenParticle>& genParts)
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

void SCRegTreeMaker::endJob()
{ 

}


  


//define this as a plug-in
DEFINE_FWK_MODULE(SCRegTreeMaker);
