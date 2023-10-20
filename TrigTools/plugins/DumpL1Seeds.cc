

#include "SHarper/TrigTools/interface/TrigToolsStructs.h"
#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "DataFormats/Math/interface/deltaR.h"
#include <iostream>

//this is a simple class which simulates another trigger based on the already calculated trigger object P4s and filters on that

#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TTree.h"

#include <vector>
#include <string>


 

class DumpL1Seeds : public edm::one::EDAnalyzer<> {


private:
  TTree* tree_;
  edm::InputTag trigEventTag_;
  edm::InputTag l1EGTag_;
  edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;
  edm::EDGetTokenT<l1t::EGammaBxCollection> l1EGToken_;
  std::string filterNameUnseeded_;
  std::string filterNameSeeded_;
  float minL1EGEt_;
  
  
public:
  explicit DumpL1Seeds(const edm::ParameterSet& iPara);
  ~DumpL1Seeds(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  
  std::pair<const l1t::EGamma*,double> matchL1(const math::XYZTLorentzVector& cand,const l1t::EGammaBxCollection& egs,const double minL1Et,const double maxDR);
  std::pair<const math::XYZTLorentzVector*,double> matchHLT(const math::XYZTLorentzVector& cand,const std::vector<math::XYZTLorentzVector>& hltObjs,const double maxDR);


};


DumpL1Seeds::DumpL1Seeds(const edm::ParameterSet& iPara)
{
  trigEventTag_ = iPara.getParameter<edm::InputTag>("trigEventTag");
  trigEventToken_ = consumes<trigger::TriggerEvent>(trigEventTag_);
  filterNameUnseeded_ = iPara.getParameter<std::string>("filterNameUnseeded");
  filterNameSeeded_ = iPara.getParameter<std::string>("filterNameSeeded");
  l1EGTag_ = iPara.getParameter<edm::InputTag>("l1EGTag");
  l1EGToken_ =consumes<l1t::EGammaBxCollection>(l1EGTag_);
  minL1EGEt_ = iPara.getParameter<double>("minL1EGEt");
}

void DumpL1Seeds::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
 

}

void DumpL1Seeds::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  //get trigger event
  edm::Handle<trigger::TriggerEvent> trigEvent; 
  iEvent.getByToken(trigEventToken_,trigEvent);
  
  //get the p4s
  std::vector<math::XYZTLorentzVector> trigObjP4sSeeded;
  trigtools::getP4sOfObsPassingFilter(trigObjP4sSeeded,*trigEvent,filterNameSeeded_,trigEventTag_.process()); 

  std::vector<math::XYZTLorentzVector> trigObjP4sUnseeded;
  trigtools::getP4sOfObsPassingFilter(trigObjP4sUnseeded,*trigEvent,filterNameUnseeded_,trigEventTag_.process()); 
  
  edm::Handle<l1t::EGammaBxCollection> l1EGs;
  iEvent.getByToken(l1EGToken_,l1EGs);
  
  std::cout <<"start L1Seed Dumper Debug Output event "<<iEvent.id().event()<<std::endl;
  std::cout <<" seeded cands "<<std::endl;
  for(const auto& cand : trigObjP4sSeeded){
    std::cout <<"    et "<<cand.pt()<<" eta "<<cand.eta()<<" phi "<<cand.phi()<<std::endl;
  }
  std::cout <<" unseeded cands "<<std::endl;
  for(const auto& cand : trigObjP4sUnseeded){
    std::cout <<"    et "<<cand.pt()<<" eta "<<cand.eta()<<" phi "<<cand.phi()<<std::endl;
  }
  std::cout <<"   L1 EG candidates "<<std::endl;
  for(int bx=-2;bx<=2;bx++){
    for(auto cand =l1EGs->begin(bx);cand!=l1EGs->end(bx);++cand){
      std::cout <<"    bx "<<bx<<" et "<<cand->pt()<<" eta "<<cand->eta()<<" phi "<<cand->phi()<<std::endl;
    }
  }

  

}

std::pair<const math::XYZTLorentzVector*,double> DumpL1Seeds::matchHLT(const math::XYZTLorentzVector& cand,const std::vector<math::XYZTLorentzVector>& hltObjs,const double maxDR)
{
  const float candEta = cand.eta();
  const float candPhi = cand.phi();
  const math::XYZTLorentzVector* bestObj = nullptr;
  double bestDR2=maxDR*maxDR;
  for(auto& hltObj : hltObjs){
    double dR2 = reco::deltaR2(candEta,candPhi,hltObj.eta(),hltObj.phi());
    if(dR2<bestDR2){
      bestDR2= dR2;
      bestObj = &hltObj;
    }
  }
  return {bestObj,std::sqrt(bestDR2)};

}

std::pair<const l1t::EGamma*,double> DumpL1Seeds::matchL1(const math::XYZTLorentzVector& cand,const l1t::EGammaBxCollection& egs,const double minL1Et,const double maxDR)
{
  const float candEta = cand.eta();
  const float candPhi = cand.phi();
  const l1t::EGamma* bestEG = nullptr;
  double bestDR2=maxDR*maxDR;
  for(auto egIt = egs.begin(0);egIt!=egs.end(0);++egIt){
    if(egIt->pt()<minL1Et) continue;
    double dR2 = reco::deltaR2(candEta,candPhi,egIt->eta(),egIt->phi());
    if(dR2<bestDR2){
      bestDR2= dR2;
      bestEG = &*egIt;
    }
  }
  return {bestEG,std::sqrt(bestDR2)};

}

//define this as a plug-in
DEFINE_FWK_MODULE(DumpL1Seeds);
