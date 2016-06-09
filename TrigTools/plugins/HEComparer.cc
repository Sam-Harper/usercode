

#include "SHarper/TrigTools/interface/TrigToolsStructs.h"
#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "DataFormats/Math/interface/deltaR.h"
#include <iostream>

//this is a simple class which simulates another trigger based on the already calculated trigger object P4s and filters on that

#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"


#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/Common/interface/AssociationMap.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"

#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/EgammaReco/interface/ElectronSeedFwd.h"


#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TTree.h"

#include <vector>
#include <string>


 

class HEComparer : public edm::EDAnalyzer {
public:
  using RecoEcalCandMap = edm::AssociationMap<edm::OneToValue<std::vector<reco::RecoEcalCandidate>, float > >;
  using CandSeedVec =  std::vector<std::pair<reco::RecoEcalCandidateRef,const reco::ElectronSeed*> >;
 
private:
  
  TTree* tree_;
  trigtools::EvtInfoStruct evtInfo_; 
  trigtools::P4Struct candP4_;
  float hademCand_;
  float hademSeed_;
  std::string treeName_;

  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> candsToken_;
  edm::EDGetTokenT<RecoEcalCandMap> hademToken_;
  edm::EDGetTokenT<reco::ElectronSeedCollection> seedsToken_;
 
  
public:
  explicit HEComparer(const edm::ParameterSet& iPara);
  ~HEComparer(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,edm::InputTag tag){token=consumes<T>(tag);}
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tag){token=consumes<T>(iPara.getParameter<edm::InputTag>(tag));}
  CandSeedVec matchSeedsAndCands(const edm::Handle<reco::RecoEcalCandidateCollection>& cands,
				 const edm::Handle<reco::ElectronSeedCollection>& seeds);

  			
  bool compareHE(CandSeedVec& candsAndSeeds,
		 edm::Handle<RecoEcalCandMap> hademValues);
  float getHadem(reco::RecoEcalCandidateRef cand,
		 edm::Handle<RecoEcalCandMap> hademValues);
};



HEComparer::HEComparer(const edm::ParameterSet& iPara)
{
  treeName_ = iPara.getParameter<std::string>("treeName");
  setToken(candsToken_,iPara,"cands");
  setToken(seedsToken_,iPara,"seeds");
  setToken(hademToken_,iPara,"hadem");  
}

void HEComparer::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_= new TTree(treeName_.c_str(),"hlt trigger");
  tree_->Branch("evt",&evtInfo_,trigtools::EvtInfoStruct::contents().c_str());
  tree_->Branch("candP4",&candP4_,trigtools::P4Struct::contents().c_str());
  tree_->Branch("hademCand",&hademCand_,"hademCand/F");
  tree_->Branch("hademSeed",&hademSeed_,"hademSeed/F");
  

}
template<typename T> edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token)
{
  edm::Handle<T> handle;
  iEvent.getByToken(token,handle);
  return handle;
}
  
const reco::ElectronSeed* matchSeed(reco::RecoEcalCandidateRef cand,
				    const edm::Handle<reco::ElectronSeedCollection>& seeds)
{
  for(auto& seed : *seeds){
    edm::RefToBase<reco::CaloCluster> caloClusterRef = seed.caloCluster() ;
    reco::SuperClusterRef scRef = caloClusterRef.castTo<reco::SuperClusterRef>() ;
    if(&(*scRef)== &(*cand->superCluster())){
      return &seed;
    }
    
  }
  return nullptr;
}

const reco::RecoEcalCandidate* matchCand(const reco::ElectronSeed& seed,
					 const edm::Handle<reco::RecoEcalCandidateCollection>& cands)
{
  for(auto& cand : *cands){
    edm::RefToBase<reco::CaloCluster> caloClusterRef = seed.caloCluster() ;
    reco::SuperClusterRef scRef = caloClusterRef.castTo<reco::SuperClusterRef>() ;
    if(&(*scRef)== &(*cand.superCluster())){
      return &cand;
    }
    
  }
  return nullptr;
}

HEComparer::CandSeedVec
HEComparer::matchSeedsAndCands(const edm::Handle<reco::RecoEcalCandidateCollection>& cands,
			       const edm::Handle<reco::ElectronSeedCollection>& seeds)
{
  CandSeedVec candsAndSeeds;
  for(size_t candNr=0;candNr<cands->size();candNr++){
    auto candRef = edm::Ref<reco::RecoEcalCandidateCollection>(cands,candNr);
    auto seed = matchSeed(candRef,seeds);
    //    if(!seed){
    // std::cout <<"Error, cand "<<candRef->pt()<<" "<<candRef->eta()<<" "<<candRef->phi()<<" does not have a seed"<<std::endl;
    //}
    candsAndSeeds.push_back({candRef,seed});
  }
  for(auto& seed : *seeds){
    auto cand = matchCand(seed,cands);
    
    if(!cand) std::cout <<"Error, seed "<<seed.caloCluster()->energy()<<" "<<seed.caloCluster()->eta()<<" "<<seed.caloCluster()->phi()<<" does not have a candidate"<<std::endl;
  }
  return candsAndSeeds;
  
}
		
float HEComparer::getHadem(reco::RecoEcalCandidateRef cand,
			   edm::Handle<RecoEcalCandMap> hademValues)
{
  auto hademIt = hademValues->find(cand);
  if(hademIt!=hademValues->end()) return hademIt->val/cand->superCluster()->energy();
  else return -1;
}
	


			
bool HEComparer::compareHE(CandSeedVec& candsAndSeeds,
			   edm::Handle<RecoEcalCandMap> hademValues)
{
  bool missMatch=false;
  for(auto& candSeed : candsAndSeeds){
    candP4_.fill(candSeed.first->p4());
    hademCand_ = getHadem(candSeed.first,hademValues);
    hademSeed_ = candSeed.second ? candSeed.second->hoe1()+ candSeed.second->hoe2() : -1;
    tree_->Fill();
    if(std::abs(hademCand_-hademSeed_)>0.00001){
      //auto cand = candSeed.first;
      // std::cout <<"hadem miss match for E "<<cand->energy()<<" eta "<<cand->eta()<<" phi "<<cand->phi()<<" cand: hadem "<<hademCand_<<" seed: hadem "<<hademSeed_<<std::endl;
      missMatch=true;
    }
    

  }
  return missMatch;
}

void HEComparer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  auto cands = getHandle(iEvent,candsToken_);
  auto seeds = getHandle(iEvent,seedsToken_);
  auto hademValues = getHandle(iEvent,hademToken_);
  
  if(seeds.isValid() && hademValues.isValid()){
    auto candsAndSeeds = matchSeedsAndCands(cands,seeds);
    evtInfo_.fill(iEvent);
    compareHE(candsAndSeeds,hademValues);
  }

}



//define this as a plug-in
DEFINE_FWK_MODULE(HEComparer);
