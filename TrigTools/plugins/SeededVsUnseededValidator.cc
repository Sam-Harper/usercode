

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

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"


#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/Common/interface/AssociationMap.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"


#include "TTree.h"

#include <vector>
#include <string>


 

class SeededVsUnseededValidator : public edm::EDAnalyzer {
  using RecoEcalCandMap = edm::AssociationMap<edm::OneToValue<std::vector<reco::RecoEcalCandidate>, float > >;
  
private:
  TTree* tree_;

  edm::InputTag l1EGTag_;
  
  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> seededCandToken_,unseededCandToken_;
  edm::EDGetTokenT<l1t::EGammaBxCollection> l1EGToken_;
  
  std::vector<edm::InputTag> seededFilterTags_;
  std::vector<edm::InputTag> unseededFilterTags_;
  std::vector<edm::InputTag> seededVarTags_;
  std::vector<edm::InputTag> unseededVarTags_;

  std::vector<edm::EDGetTokenT<trigger::TriggerFilterObjectWithRefs>> seededFilterTokens_,unseededFilterTokens_;
  std::vector<edm::EDGetTokenT<RecoEcalCandMap>> seededVarTokens_,unseededVarTokens_;
  
  
public:
  explicit SeededVsUnseededValidator(const edm::ParameterSet& iPara);
  ~SeededVsUnseededValidator(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  
  std::pair<const l1t::EGamma*,double> matchL1(const math::XYZTLorentzVector& cand,const l1t::EGammaBxCollection& egs,const double minL1Et,const double maxDR);
  std::pair<const math::XYZTLorentzVector*,double> matchHLT(const math::XYZTLorentzVector& cand,const std::vector<math::XYZTLorentzVector>& hltObjs,const double maxDR);

  template <typename T> void setToken(edm::EDGetTokenT<T>& token,edm::InputTag tag){token=consumes<T>(tag);}
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tag){token=consumes<T>(iPara.getParameter<edm::InputTag>(tag));}
};



SeededVsUnseededValidator::SeededVsUnseededValidator(const edm::ParameterSet& iPara)
{

  setToken(l1EGToken_,iPara,"l1EGTag");
  setToken(seededCandToken_,iPara,"seededCands");
  setToken(unseededCandToken_,iPara,"unseededCands");
  seededFilterTags_=iPara.getParameter<std::vector<edm::InputTag>>("seededFilters");
  unseededFilterTags_=iPara.getParameter<std::vector<edm::InputTag>>("unseededFilters");
  seededVarTags_=iPara.getParameter<std::vector<edm::InputTag>>("seededVars");
  unseededVarTags_=iPara.getParameter<std::vector<edm::InputTag>>("unseededVars");
  
  for(const auto& tag : seededFilterTags_){
    seededFilterTokens_.push_back(consumes<trigger::TriggerFilterObjectWithRefs>(tag));
  }
  for(const auto& tag : unseededFilterTags_){
    unseededFilterTokens_.push_back(consumes<trigger::TriggerFilterObjectWithRefs>(tag));
  }
  for(const auto& tag : seededVarTags_){
    seededVarTokens_.push_back(consumes<RecoEcalCandMap>(tag));
  }
  for(const auto& tag : unseededVarTags_){
    unseededVarTokens_.push_back(consumes<RecoEcalCandMap>(tag));
  }
  
}

void SeededVsUnseededValidator::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
 

}

void SeededVsUnseededValidator::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  

}

std::pair<const math::XYZTLorentzVector*,double> SeededVsUnseededValidator::matchHLT(const math::XYZTLorentzVector& cand,const std::vector<math::XYZTLorentzVector>& hltObjs,const double maxDR)
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

std::pair<const l1t::EGamma*,double> SeededVsUnseededValidator::matchL1(const math::XYZTLorentzVector& cand,const l1t::EGammaBxCollection& egs,const double minL1Et,const double maxDR)
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
DEFINE_FWK_MODULE(SeededVsUnseededValidator);
