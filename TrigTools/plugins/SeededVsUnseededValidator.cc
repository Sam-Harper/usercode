

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

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "TTree.h"

#include <vector>
#include <string>


 

class SeededVsUnseededValidator : public edm::EDAnalyzer {
  using RecoEcalCandMap = edm::AssociationMap<edm::OneToValue<std::vector<reco::RecoEcalCandidate>, float > >;
  using FilterToken = edm::EDGetTokenT<trigger::TriggerFilterObjectWithRefs>;
  using VarToken = edm::EDGetTokenT<RecoEcalCandMap>;
  using CandToken = edm::EDGetTokenT<reco::RecoEcalCandidateCollection>;
private:
  

  edm::InputTag l1EGTag_;
  
  CandToken seededCandToken_,unseededCandToken_;
  edm::EDGetTokenT<l1t::EGammaBxCollection> l1EGToken_;
  
  std::vector<edm::InputTag> seededFilterTags_;
  std::vector<edm::InputTag> unseededFilterTags_;
  std::vector<edm::InputTag> seededVarTags_;
  std::vector<edm::InputTag> unseededVarTags_;

  std::vector<FilterToken > seededFilterTokens_,unseededFilterTokens_;
  std::vector<VarToken > seededVarTokens_,unseededVarTokens_;
  edm::ESHandle<CaloGeometry> calGeometryHandle_;
  
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
  std::pair<const reco::RecoEcalCandidate*,double> matchHLT(const reco::RecoEcalCandidateRef& cand,const std::vector<reco::RecoEcalCandidateRef>& hltObjs,const double maxDR);

  const reco::RecoEcalCandidate* matchSCBySeed(const reco::SuperCluster& sc,const std::vector<reco::RecoEcalCandidate>& cands);

  template <typename T> void setToken(edm::EDGetTokenT<T>& token,edm::InputTag tag){token=consumes<T>(tag);}
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tag){token=consumes<T>(iPara.getParameter<edm::InputTag>(tag));}
  bool lastFilterMatches(const edm::Event& iEvent);
  void printFilterInfo(const edm::Event& iEvent,FilterToken filterToken,VarToken varToken,CandToken candToken);
  
  void printSuperClusters(const std::vector<reco::RecoEcalCandidate>& seededCands,
			  const std::vector<reco::RecoEcalCandidate>& unseededCands,
			  const l1t::EGammaBxCollection& l1EGs);
  std::pair<float,float> getMaxEtaPhiRecHitDist(const reco::RecoEcalCandidate& cand,
						const l1t::EGammaBxCollection& egs);
  void printMaxRecHitDistsOfCands(const std::vector<reco::RecoEcalCandidate>& cands,
				  const l1t::EGammaBxCollection& l1EGs);
    

};



SeededVsUnseededValidator::SeededVsUnseededValidator(const edm::ParameterSet& iPara)
{

  setToken(l1EGToken_,iPara,"l1EGTag");
  setToken(seededCandToken_,iPara,"seededCands");
  setToken(unseededCandToken_,iPara,"unseededCands");
  seededFilterTags_=iPara.getParameter<std::vector<edm::InputTag> >("seededFilters");
  unseededFilterTags_=iPara.getParameter<std::vector<edm::InputTag> >("unseededFilters");
  seededVarTags_=iPara.getParameter<std::vector<edm::InputTag> >("seededVars");
  unseededVarTags_=iPara.getParameter<std::vector<edm::InputTag> >("unseededVars");
  
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
template<typename T> edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token)
{
  edm::Handle<T> handle;
  iEvent.getByToken(token,handle);
  return handle;
}
  
std::vector<reco::RecoEcalCandidateRef> getPassingEcalCands(const trigger::TriggerFilterObjectWithRefs& filterObj)
{
  std::vector<reco::RecoEcalCandidateRef> cands;
  filterObj.getObjects(trigger::TriggerCluster,cands);
  if(cands.empty()) filterObj.getObjects(trigger::TriggerPhoton,cands);
  return cands;

}

bool SeededVsUnseededValidator::lastFilterMatches(const edm::Event& iEvent)
{
  auto seededLastFilter = getHandle(iEvent,seededFilterTokens_.back());
  auto unseededLastFilter = getHandle(iEvent,unseededFilterTokens_.back());
  auto seededCands = getPassingEcalCands(*seededLastFilter);
  auto unseededCands = getPassingEcalCands(*unseededLastFilter);
  
  bool pass=true;
  for(auto unseededCand : unseededCands){
    auto match = matchHLT(unseededCand,seededCands,0.3);
    if(!match.first) pass=false;
    
  }
  return pass;
 
}



void SeededVsUnseededValidator::printFilterInfo(const edm::Event& iEvent,FilterToken filterToken,VarToken varToken,CandToken candToken)				
{
  auto filter = getHandle(iEvent,filterToken);
  auto varMap = getHandle(iEvent,varToken);
  auto passingCands = getPassingEcalCands(*filter);
  auto cands = getHandle(iEvent,candToken);

  for(size_t candNr=0;candNr<cands->size();candNr++){
    auto candRef = edm::Ref<reco::RecoEcalCandidateCollection>(cands,candNr);
    auto match = matchHLT(candRef,passingCands,0.001);
    float val = varMap.isValid() ? varMap->find(candRef)->val : -999;
    if(candRef->et()>30){
      std::cout <<"  et "<<candRef->et()<<" eta "<<candRef->eta()<<" phi "<<candRef->phi()<<" var "<<val<<" pass "<<(match.first!=nullptr)<<std::endl;
    }
  }
   
}

void SeededVsUnseededValidator::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  iSetup.get<CaloGeometryRecord>().get(calGeometryHandle_);

  bool pass = lastFilterMatches(iEvent);
  auto l1EGs = getHandle(iEvent,l1EGToken_);
  int nrEG26s = 0;
  for(auto l1EG = l1EGs->begin(0);l1EG!=l1EGs->end(0);++l1EG){
    if(l1EG->pt()>=26) nrEG26s++;
  }
  pass=true;
  if(!pass && nrEG26s>=2){


    std::cout <<"Found HLT Unseeded-Seeded Missmatch"<<std::endl;

    std::cout <<"L1 EGs"<<std::endl;
    for(auto l1EG = l1EGs->begin(0);l1EG!=l1EGs->end(0);++l1EG){
      std::cout <<"  et "<<l1EG->pt()<<" eta "<<l1EG->eta()<<" phi "<<l1EG->phi()<<std::endl;
    }

    for(size_t filterNr=0;filterNr<seededFilterTokens_.size();filterNr++){
      std::cout <<"filter "<<seededFilterTags_[filterNr].label()<<std::endl;
      printFilterInfo(iEvent,seededFilterTokens_[filterNr],seededVarTokens_[filterNr],seededCandToken_);
      std::cout <<"filter "<<unseededFilterTags_[filterNr].label()<<std::endl;
      printFilterInfo(iEvent,unseededFilterTokens_[filterNr],unseededVarTokens_[filterNr],unseededCandToken_);
    }
    printSuperClusters(*getHandle(iEvent,seededCandToken_),
		       *getHandle(iEvent,unseededCandToken_),
		       *getHandle(iEvent,l1EGToken_));  

  }
  printMaxRecHitDistsOfCands(*getHandle(iEvent,unseededCandToken_),
			       *getHandle(iEvent,l1EGToken_)); 

}



void SeededVsUnseededValidator::
printMaxRecHitDistsOfCands(const std::vector<reco::RecoEcalCandidate>& cands,
			   const l1t::EGammaBxCollection& l1EGs)
{
  std::cout <<"printing max dEta,dPhi"<<std::endl;
  for(auto& cand : cands){
    
    getMaxEtaPhiRecHitDist(cand,l1EGs);
  }
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
const reco::RecoEcalCandidate*
SeededVsUnseededValidator::matchSCBySeed(const reco::SuperCluster& sc,const std::vector<reco::RecoEcalCandidate>& cands)
{
  for(auto& cand : cands){
    if(cand.superCluster()->seed()->seed()==sc.seed()->seed()) return &cand;
  }
  return nullptr;
    
}

std::pair<const reco::RecoEcalCandidate*,double> 
SeededVsUnseededValidator::matchHLT(const reco::RecoEcalCandidateRef& cand,const std::vector<reco::RecoEcalCandidateRef>& hltObjs,const double maxDR)
{
  const float candEta = cand->eta();
  const float candPhi = cand->phi();
  const reco::RecoEcalCandidate* bestObj = nullptr;
  double bestDR2=maxDR*maxDR;
  for(auto& hltObj : hltObjs){
    double dR2 = reco::deltaR2(candEta,candPhi,hltObj->eta(),hltObj->phi());
    if(dR2<bestDR2){
      bestDR2= dR2;
      bestObj = &*hltObj;
    }
  }
  return {bestObj,std::sqrt(bestDR2)};

}


void SeededVsUnseededValidator::
printSuperClusters(const std::vector<reco::RecoEcalCandidate>& seededCands,
		   const std::vector<reco::RecoEcalCandidate>& unseededCands,
		   const l1t::EGammaBxCollection& l1EGs)
{
  for(const auto& unseededCand : unseededCands){
    reco::SuperClusterRef unseededSC = unseededCand.superCluster();
    auto matchedCand = matchSCBySeed(*unseededSC,seededCands);
    reco::SuperClusterRef matchedSC;
    if(matchedCand) matchedSC = matchedCand->superCluster();
    if(matchedSC.isNonnull() && std::abs(unseededSC->energy()-matchedSC->energy())>0.00001){
      std::cout <<"supercluster energy difference "<<std::endl;
      auto printSC =[](const reco::SuperCluster& sc){
	std::cout <<"energy "<<sc.energy()<<" raw nrgy "<<sc.rawEnergy()<<" ps nrgy "<<sc.preshowerEnergy()<<" eta "<<sc.eta()<<" phi "<<sc.phi()<<" nr ps clus "<<sc.preshowerClustersSize()<<" nr sub clus "<<sc.clustersSize()<<std::endl;
	for(auto bc = sc.clustersBegin();bc!=sc.clustersEnd();++bc){
	  std::cout <<"   clus : "<<(*bc)->energy()<<" , "<<(*bc)->eta()<<" , "<<(*bc)->phi()<<std::endl;//<<" "<<**bc<<std::endl;
	}
      };
      std::cout <<"unseeded "<<std::endl;
      printSC(*unseededSC);
      std::cout <<"seeded "<<std::endl;
      printSC(*matchedSC);
      auto maxDiff = getMaxEtaPhiRecHitDist(unseededCand,l1EGs);
      
      std::cout <<" unseeded max diff eta "<<maxDiff.first<<" phi "<<maxDiff.second<<std::endl;   
    }
  }
}



std::pair<float,float> 
SeededVsUnseededValidator::
getMaxEtaPhiRecHitDist(const reco::RecoEcalCandidate& cand,
		       const l1t::EGammaBxCollection& egs)
{
  auto matchedEG = matchL1(cand.p4(),egs,0,0.2);
  if(matchedEG.first==nullptr) return {-1,-1};
  float l1Eta = matchedEG.first->eta();
  float l1Phi = matchedEG.first->phi();
  
  float maxEtaDiff=0.;
  float maxPhiDiff=0.;
  std::pair<DetId,float> maxEtaHit;
  std::pair<DetId,float> maxPhiHit;
    

  reco::SuperClusterRef superClus = cand.superCluster();

  for(auto subClus = superClus->clustersBegin();subClus!=superClus->clustersEnd();++subClus){
    for(auto& hit : (*subClus)->hitsAndFractions()){
      const CaloSubdetectorGeometry* subDetGeom =  calGeometryHandle_->getSubdetectorGeometry(hit.first);
      const CaloCellGeometry* cellGeom = subDetGeom ? subDetGeom->getGeometry(hit.first) : nullptr;
      if(cellGeom){
	const GlobalPoint& cellPos =cellGeom->getPosition();
	float dPhi = std::abs(reco::deltaPhi(l1Phi,cellPos.phi()));
	float dEta = std::abs(l1Eta-cellPos.eta());
	if(dPhi>maxPhiDiff){
	  maxPhiDiff=dPhi;
	  maxPhiHit=hit;
	}
	if(dEta>maxEtaDiff){
	  maxEtaDiff=dEta;
	  maxEtaHit=hit;
	}
	
      }else{
	std::cout <<"Error, didnt find cell geom for "<<hit.first.rawId()<<std::endl;
      }
    }
  }
  std::cout <<"  et "<<cand.et()<<" eta "<<cand.eta()<<" phi "<<cand.phi()<<" l1 "<<matchedEG.first->pt()<<" eta "<<l1Eta<<" phi "<<l1Phi<<" max dEta "<<maxEtaDiff<<",frac "<<maxEtaHit.second<<" dPhi "<<maxPhiDiff<<", frac "<<maxPhiHit.second<<std::endl;

  return {maxEtaDiff,maxPhiDiff};

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
