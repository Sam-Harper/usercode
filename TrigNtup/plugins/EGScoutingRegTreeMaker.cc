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
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/Scouting/interface/Run3ScoutingElectron.h"
#include "DataFormats/Scouting/interface/Run3ScoutingVertex.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h" 


#include "SHarper/TrigNtup/interface/EGRegTreeStruct.hh"
#include "SHarper/TrigNtup/interface/EGScoutingRegTreeStruct.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>

class EGScoutingRegTreeMaker : public edm::one::EDAnalyzer<> {

private:
  EGScoutingRegTreeStruct egRegTreeData_;
  TTree* egRegTree_;
  std::string treeName_;

  edm::EDGetTokenT<std::vector<Run3ScoutingVertex>>  scoutVtxToken_;
  edm::EDGetTokenT<double> rhoToken_;
  edm::EDGetTokenT<reco::GenParticleCollection> genPartsToken_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > puSumToken_;
  edm::EDGetTokenT<std::vector<Run3ScoutingElectron>> scoutElesToken_;
  EGScoutingRegTreeMaker(const EGScoutingRegTreeMaker& rhs)=delete;
  EGScoutingRegTreeMaker& operator=(const EGScoutingRegTreeMaker& rhs)=delete;

public:
  explicit EGScoutingRegTreeMaker(const edm::ParameterSet& iPara);
  virtual ~EGScoutingRegTreeMaker();
  
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
  template<typename T>
  const T* matchToGen(float eta,float phi,const std::vector<T >& objs,float maxDR=0.1);
};



EGScoutingRegTreeMaker::EGScoutingRegTreeMaker(const edm::ParameterSet& iPara):
  egRegTree_(nullptr),
  treeName_("egScoutRegTree")
{
  if(iPara.exists("treeName")){
    treeName_ = iPara.getParameter<std::string>("treeName");
  }
  setToken(scoutVtxToken_,iPara,"verticesTag");
  setToken(rhoToken_,iPara,"rhoTag");
  setToken(genPartsToken_,iPara,"genPartsTag");
  setToken(puSumToken_,iPara,"puSumTag");
  setToken(scoutElesToken_,iPara,"scoutElesTag");
  

}

EGScoutingRegTreeMaker::~EGScoutingRegTreeMaker()
{

}


void EGScoutingRegTreeMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  egRegTree_ = new TTree(treeName_.c_str(),"");
  egRegTreeData_.createBranches(egRegTree_);
} 

void EGScoutingRegTreeMaker::beginRun(const edm::Run& run,const edm::EventSetup& iSetup)
{ 
 
}

namespace {
  
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
  

void EGScoutingRegTreeMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  auto genPartsHandle = iEvent.getHandle(genPartsToken_);
  auto verticesHandle = iEvent.getHandle(scoutVtxToken_);
  auto rhoHandle = iEvent.getHandle(rhoToken_);
  auto scoutElesHandle = iEvent.getHandle(scoutElesToken_);
  auto puSumHandle = iEvent.getHandle(puSumToken_);

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

  for(const auto& genPart : *genPartsHandle){
    if((std::abs(genPart.pdgId())==11 || genPart.pdgId()==22) && genPart.statusFlags().isPrompt() && genPart.statusFlags().isFirstCopy()){
      const Run3ScoutingElectron* scoutEle = matchToGen(genPart.eta(),genPart.phi(),*scoutElesHandle);

      egRegTreeData_.fill(iEvent,nrVert,*rhoHandle,nrPUInt,nrPUIntTrue,
			  &genPart,scoutEle);
      egRegTree_->Fill();
    }
  }
    

}

const reco::GenParticle*  EGScoutingRegTreeMaker::matchGenPart(float eta,float phi,const std::vector<reco::GenParticle>& genParts)
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



template<typename T>
const T* EGScoutingRegTreeMaker::matchToGen(float eta,float phi,const std::vector<T>& objs,float maxDR)
{
  const T* bestMatch=nullptr;
  float bestDR2=maxDR*maxDR;

  for(const auto& obj: objs){
      float dR2 = reco::deltaR2(obj.eta(),obj.phi(),eta,phi);
      if(dR2<bestDR2){
	bestMatch = &obj;
	bestDR2 = dR2;
      }
  }
  return bestMatch;
}

void EGScoutingRegTreeMaker::endJob()
{ 

}


  


//define this as a plug-in
DEFINE_FWK_MODULE(EGScoutingRegTreeMaker);
