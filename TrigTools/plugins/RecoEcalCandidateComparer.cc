#ifndef SHARPER_TRIGTOOLS_ECALRECHITCOLLCOMPARER
#define SHARPER_TRIGTOOLS_ECALRECHITCOLLCOMPARER

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "SHarper/TrigTools/interface/TrigToolsStructs.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "TTree.h"

#include <vector>
#include <string>
#include <iostream>

namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}

const reco::RecoEcalCandidate* matchRecoEcalCandidate(const math::XYZTLorentzVector& p4, const reco::RecoEcalCandidateCollection &ecalCands);
std::vector<std::pair<const math::XYZTLorentzVector*,const math::XYZTLorentzVector*> > 
matchObjs(const std::vector<math::XYZTLorentzVector>& trigObjsColl1,const std::vector<math::XYZTLorentzVector>& trigObjsColl2);

std::vector<std::pair<const reco::RecoEcalCandidate*,const reco::RecoEcalCandidate*> >
matchEcalCands(const std::vector<std::pair<const math::XYZTLorentzVector*,const math::XYZTLorentzVector*> >& matchedObjs,
	       const reco::RecoEcalCandidateCollection &coll1,
	       const reco::RecoEcalCandidateCollection &coll2);

class RecoEcalCandidateComparer : public edm::EDAnalyzer {
private:

  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> coll1Token_;
  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> coll2Token_;
  edm::EDGetTokenT<trigger::TriggerEvent> trigEvtToken_;
  edm::InputTag coll1Tag_;
  edm::InputTag coll2Tag_;
  edm::InputTag trigEvtTag_;
  const std::string trigColl1_;
  const std::string trigColl2_;


 
public:
  explicit RecoEcalCandidateComparer(const edm::ParameterSet& iPara);
  ~RecoEcalCandidateComparer(){}
  void beginJob() override;
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)override;
  //  virtual bool filter(edm::Event& iEvent,const edm::EventSetup& iSetup)override;


};

RecoEcalCandidateComparer::RecoEcalCandidateComparer(const edm::ParameterSet& iPara):
  trigColl1_(iPara.getParameter<std::string>("trigColl1")),
  trigColl2_(iPara.getParameter<std::string>("trigColl2"))
{
  coll1Tag_=iPara.getParameter<edm::InputTag>("collection1");
  coll2Tag_=iPara.getParameter<edm::InputTag>("collection2");
  
  coll1Token_=consumes<reco::RecoEcalCandidateCollection>(coll1Tag_);
  coll2Token_=consumes<reco::RecoEcalCandidateCollection>(coll2Tag_);
  trigEvtTag_=iPara.getParameter<edm::InputTag>("trigEvent");
  trigEvtToken_=consumes<trigger::TriggerEvent>(trigEvtTag_);
  
}

void RecoEcalCandidateComparer::beginJob()
{

}

void RecoEcalCandidateComparer::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  edm::Handle<reco::RecoEcalCandidateCollection> coll1Handle,coll2Handle;
  edm::Handle<trigger::TriggerEvent> trigEvtHandle;
  
  iEvent.getByToken(coll1Token_,coll1Handle);
  iEvent.getByToken(coll2Token_,coll2Handle);
  iEvent.getByToken(trigEvtToken_,trigEvtHandle);

  std::vector<math::XYZTLorentzVector> trigObjsColl1,trigObjsColl2;
  trigtools::getP4sOfObsPassingFilter(trigObjsColl1,*trigEvtHandle,trigColl1_,trigEvtTag_.process()); 
  trigtools::getP4sOfObsPassingFilter(trigObjsColl2,*trigEvtHandle,trigColl2_,trigEvtTag_.process()); 

  std::vector<std::pair<const math::XYZTLorentzVector*,const math::XYZTLorentzVector*> > matchedObjs = matchObjs(trigObjsColl1,trigObjsColl2);

  std::vector<std::pair<const reco::RecoEcalCandidate*,const reco::RecoEcalCandidate*> > matchedEcalCands = matchEcalCands(matchedObjs,*coll1Handle,*coll2Handle);
  std::cout <<iEvent.id().event()<<" lumi "<<iEvent.luminosityBlock()<<"nr cands "<<matchedEcalCands.size()<<std::endl;
  for(auto& candPair : matchedEcalCands){
    std::cout <<"E "<<candPair.first->energy()<<" "<<candPair.second->energy()<<std::endl;
    std::cout <<"Et"<<candPair.first->et()<<" "<<candPair.second->et()<<std::endl;
    if(std::abs(candPair.first->energy()-candPair.second->energy())>1){
      std::cout <<"miss match "<<candPair.first->energy()<<" "<<candPair.second->energy()<<std::endl;
      auto printSC = [](const reco::SuperClusterRef& clus){
	std::cout <<"  clus raw E "<<clus->rawEnergy()<<" nrclus "<<clus->clustersSize()<<" size "<<clus->size()<<std::endl;
      };
      printSC(candPair.first->superCluster());
      printSC(candPair.second->superCluster());
    }
       
  }
  
}

std::vector<std::pair<const reco::RecoEcalCandidate*,const reco::RecoEcalCandidate*> >
matchEcalCands(const std::vector<std::pair<const math::XYZTLorentzVector*,const math::XYZTLorentzVector*> >& matchedObjs,
	       const reco::RecoEcalCandidateCollection &coll1,
	       const reco::RecoEcalCandidateCollection &coll2)
{
  std::vector<std::pair<const reco::RecoEcalCandidate*,const reco::RecoEcalCandidate*> > res;
  for(const auto& p4Pair : matchedObjs){
    auto matchedCand1 = matchRecoEcalCandidate(*p4Pair.first,coll1);
    auto matchedCand2 = matchRecoEcalCandidate(*p4Pair.second,coll2);
    if(!matchedCand1 || !matchedCand2){
      std::cout <<" warning no match "<<std::endl;
    }else{
      res.push_back(std::make_pair(matchedCand1,matchedCand2));
    }
  }
  return res;
}
  
const reco::RecoEcalCandidate* matchRecoEcalCandidate(const math::XYZTLorentzVector& p4, const reco::RecoEcalCandidateCollection &ecalCands)
{
  const reco::RecoEcalCandidate* bestMatch=nullptr;
  float bestDR2=0.1*0.1;
  for(auto& cand : ecalCands){
    float dR2 = reco::deltaR2(p4.eta(),p4.phi(),cand.eta(),cand.phi());
    if(dR2<bestDR2){
      bestMatch=&cand;
      bestDR2=dR2;
    }
  }
  return bestMatch;
}

std::vector<std::pair<const math::XYZTLorentzVector*,const math::XYZTLorentzVector*> > 
matchObjs(const std::vector<math::XYZTLorentzVector>& trigObjsColl1,const std::vector<math::XYZTLorentzVector>& trigObjsColl2)
{

  std::vector<std::pair<const math::XYZTLorentzVector*,const math::XYZTLorentzVector*> > res;
  for(auto& obj1 : trigObjsColl1) {
    
    float bestDR2 = 0.1*0.1;
    const math::XYZTLorentzVector* bestMatch=nullptr;
    for(auto & obj2 : trigObjsColl2) {
      float dR2 = reco::deltaR2(obj1.eta(),obj1.phi(),obj2.eta(),obj2.phi());
      if(dR2<bestDR2){
	bestMatch=&obj2;
	bestDR2=dR2;
      }
    }
    if(bestMatch) res.push_back(std::make_pair(&obj1,bestMatch));
  }
  return res;

}



DEFINE_FWK_MODULE(RecoEcalCandidateComparer);
#endif
