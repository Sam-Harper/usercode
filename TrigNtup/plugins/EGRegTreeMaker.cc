#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
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
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h" 

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"	
#include "Geometry/Records/interface/CaloTopologyRecord.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"

#include "SHarper/TrigNtup/interface/EGRegTreeStruct.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>

class EGRegTreeMaker : public edm::one::EDAnalyzer<> {

private:
  EGRegTreeStruct egRegTreeData_;
  TTree* egRegTree_;
  std::string treeName_;

  edm::EDGetTokenT<reco::VertexCollection>  verticesToken_;
  edm::EDGetTokenT<double> rhoToken_;
  edm::EDGetTokenT<reco::GenParticleCollection> genPartsToken_;
  std::vector<edm::EDGetTokenT<reco::SuperClusterCollection>> scTokens_;
  std::vector<edm::EDGetTokenT<reco::SuperClusterCollection>> scAltTokens_;
  edm::EDGetTokenT<EcalRecHitCollection> ecalHitsEBToken_;
  edm::EDGetTokenT<EcalRecHitCollection> ecalHitsEEToken_;
  edm::EDGetTokenT<std::vector<reco::GsfElectron> > elesToken_;
  edm::EDGetTokenT<std::vector<reco::Photon> > phosToken_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > puSumToken_;

  std::vector<edm::EDGetTokenT<std::vector<reco::GsfElectron> > > eleAltTokens_;
  std::vector<edm::EDGetTokenT<std::vector<reco::Photon> > > phoAltTokens_;

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
  const reco::SuperCluster*  matchSC(const reco::SuperCluster* scToMatch,const std::vector<edm::Handle<reco::SuperClusterCollection> >& scHandles);
  static const reco::SuperCluster*  matchSC(float eta,float phi,const std::vector<edm::Handle<reco::SuperClusterCollection> >& scHandles);
};



EGRegTreeMaker::EGRegTreeMaker(const edm::ParameterSet& iPara):
  egRegTree_(nullptr),
  treeName_("egRegTree")
{
  if(iPara.exists("treeName")){
    treeName_ = iPara.getParameter<std::string>("treeName");
  }
  setToken(verticesToken_,iPara,"verticesTag");
  setToken(rhoToken_,iPara,"rhoTag");
  setToken(genPartsToken_,iPara,"genPartsTag");
  setToken(scTokens_,iPara,"scTag");
  setToken(scAltTokens_,iPara,"scAltTag");
  setToken(ecalHitsEBToken_,iPara,"ecalHitsEBTag");
  setToken(ecalHitsEEToken_,iPara,"ecalHitsEETag");
  setToken(elesToken_,iPara,"elesTag");
  setToken(phosToken_,iPara,"phosTag");
  setToken(puSumToken_,iPara,"puSumTag");
  setToken(eleAltTokens_,iPara,"elesAltTag");
  setToken(phoAltTokens_,iPara,"phosAltTag");

}

EGRegTreeMaker::~EGRegTreeMaker()
{

}


void EGRegTreeMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  egRegTree_ = new TTree(treeName_.c_str(),"");
  egRegTreeData_.setNrEnergies(eleAltTokens_.size(),phoAltTokens_.size());
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
  template<typename T> 
  const T* matchBySCSeedId(unsigned int seedId,const std::vector<T>& objs){
    for(auto& obj : objs){
      if(obj.superCluster()->seed()->seed().rawId()==seedId) return &obj;
    }
    return nullptr;
  }
  const reco::GsfElectron* matchEle(unsigned int seedId,const std::vector<reco::GsfElectron>& eles){
    return matchBySCSeedId(seedId,eles);
  }
  const reco::Photon* matchPho(unsigned int seedId,const std::vector<reco::Photon>& phos){
    return matchBySCSeedId(seedId,phos);
  }
  
  template<typename T> std::vector<const T*> 
  matchToAltCollsBySCSeedId(const T* objToMatch,const std::vector<edm::Handle<std::vector<T> > >& objCollHandles){
    std::vector<const T*> matches;
    if(objToMatch){
      unsigned int seedId = objToMatch->superCluster()->seed()->seed().rawId();
      for(auto& objCollHandle : objCollHandles){
	if(objCollHandle.isValid()) matches.push_back(matchBySCSeedId(seedId,*objCollHandle));
	else matches.push_back(nullptr);
      }
    }
    else{
      matches.resize(objCollHandles.size(),nullptr);
    }
    return matches;
  }
}

void EGRegTreeMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  auto scHandles = getHandle(iEvent,scTokens_);
  auto scAltHandles = getHandle(iEvent,scAltTokens_);
  auto ecalHitsEBHandle = getHandle(iEvent,ecalHitsEBToken_);
  auto ecalHitsEEHandle = getHandle(iEvent,ecalHitsEEToken_);
  auto genPartsHandle = getHandle(iEvent,genPartsToken_);
  auto verticesHandle = getHandle(iEvent,verticesToken_);
  auto rhoHandle = getHandle(iEvent,rhoToken_);
  auto elesHandle = getHandle(iEvent,elesToken_);
  auto eleAltHandles = getHandle(iEvent,eleAltTokens_);
  auto phosHandle = getHandle(iEvent,phosToken_);
  auto phoAltHandles = getHandle(iEvent,phoAltTokens_);
  auto puSumHandle = getHandle(iEvent,puSumToken_);
  edm::ESHandle<CaloTopology> caloTopoHandle;
  iSetup.get<CaloTopologyRecord>().get(caloTopoHandle);
  edm::ESHandle<EcalChannelStatus> chanStatusHandle;
  iSetup.get<EcalChannelStatusRcd>().get(chanStatusHandle);

  int nrVert = verticesHandle->size();
  float nrPUInt = -1;
  float nrPUIntTrue = -1;
  if(puSumHandle.isValid()){
    for(auto& puInfo : *puSumHandle){
      if(puInfo.getBunchCrossing()==0){
	nrPUInt = puInfo.getPU_NumInteractions();
	nrPUIntTrue = puInfo.getTrueNumInteractions();
	break;
      }
    }
  }

  bool fillFromSC = false;
  if(fillFromSC){
    for(const auto& scHandle : scHandles){
      for(const auto& sc: *scHandle){
	const reco::GenParticle* genPart = genPartsHandle.isValid() ? matchGenPart(sc.eta(),sc.phi(),*genPartsHandle) : nullptr;
	const reco::GsfElectron* ele = elesHandle.isValid() ? matchEle(sc.seed()->seed().rawId(),*elesHandle) : nullptr;
	const reco::Photon* pho = phosHandle.isValid() ? matchPho(sc.seed()->seed().rawId(),*phosHandle) : nullptr;
	const reco::SuperCluster* scAlt = matchSC(&sc,scAltHandles);
	
	const std::vector<const reco::GsfElectron*> altEles = matchToAltCollsBySCSeedId(ele,eleAltHandles);
	const std::vector<const reco::Photon*> altPhos = matchToAltCollsBySCSeedId(pho,phoAltHandles);

	if(hasBasicClusters(sc)){
	  egRegTreeData_.fill(iEvent,nrVert,*rhoHandle,nrPUInt,nrPUIntTrue,
			      *ecalHitsEBHandle,*ecalHitsEEHandle,
			      *caloTopoHandle,
			      *chanStatusHandle,
			      &sc,genPart,ele,pho,scAlt,
			      altEles,altPhos);
	  egRegTree_->Fill();
	}
      }
    }
  }else{

    for(const auto& genPart : *genPartsHandle){
      if((std::abs(genPart.pdgId())==11 || genPart.pdgId()==22) && genPart.statusFlags().isPrompt() && genPart.statusFlags().isFirstCopy()){
	const reco::SuperCluster* sc = matchSC(genPart.eta(),genPart.phi(),scHandles);
	const reco::SuperCluster* scAlt = matchSC(sc,scAltHandles);
	const reco::GsfElectron* ele = elesHandle.isValid() && sc ? matchEle(sc->seed()->seed().rawId(),*elesHandle) : nullptr;
	const reco::Photon* pho = phosHandle.isValid() && sc ? matchPho(sc->seed()->seed().rawId(),*phosHandle) : nullptr;
	
	const std::vector<const reco::GsfElectron*> altEles = matchToAltCollsBySCSeedId(ele,eleAltHandles);
	const std::vector<const reco::Photon*> altPhos = matchToAltCollsBySCSeedId(pho,phoAltHandles);


	egRegTreeData_.fill(iEvent,nrVert,*rhoHandle,nrPUInt,nrPUIntTrue,
			    *ecalHitsEBHandle,*ecalHitsEEHandle,
			    *caloTopoHandle,
			    *chanStatusHandle,
			    sc,&genPart,ele,pho,scAlt,
			    altEles,altPhos);
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


//matched by seed det id which should be unique
const reco::SuperCluster*  EGRegTreeMaker::matchSC(const reco::SuperCluster* scToMatch,const std::vector<edm::Handle<reco::SuperClusterCollection> >& scHandles)
{
  if(scToMatch){
    auto seedId = scToMatch->seed()->seed().rawId();
    for(const auto& scHandle : scHandles){
      if(scHandle.isValid()){
	for(const auto& sc: *scHandle){
	  if(sc.seed()->seed().rawId()==seedId){
	    return &sc;
	  }
	}
      }
    }
  }
  return nullptr;
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
