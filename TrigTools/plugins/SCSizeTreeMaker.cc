

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
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "TTree.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include <vector>
#include <string>


 

class SCSizeTreeMaker : public edm::EDAnalyzer {
private:
  using RecoEcalCandMap = edm::AssociationMap<edm::OneToValue<std::vector<reco::RecoEcalCandidate>, float > >;
  using CandToken = edm::EDGetTokenT<reco::RecoEcalCandidateCollection>;

  class MaxDiffHitFinder {
  private:
    double maxPosDiff_;
    double maxNegDiff_;
    std::pair<DetId,float> maxPosDiffHit_;
    std::pair<DetId,float> maxNegDiffHit_;
    std::function<double(const GlobalPoint&)> diffFunc_;
  public:
    MaxDiffHitFinder(std::function<double(const GlobalPoint&)>&& iDiffFunc):
      diffFunc_(iDiffFunc){
      reset();
    }
    ~MaxDiffHitFinder(){}
    
    const std::pair<DetId,float>& maxPosDiffHit()const{return maxPosDiffHit_;}
    const std::pair<DetId,float>& maxNegDiffHit()const{return maxNegDiffHit_;}
    double maxPosDiff()const{return maxPosDiff_;}
    double maxNegDiff()const{return maxNegDiff_;}

    void reset(){
      maxPosDiff_=-1.;
      maxNegDiff_=1.;
      maxPosDiffHit_.first=0;
      maxPosDiffHit_.second=-1.;
      maxNegDiffHit_.first=0;
      maxNegDiffHit_.second=-1.;

    }
    void operator()(const std::pair<DetId,float>& hit,const GlobalPoint& point){
      if(hit.first==maxNegDiffHit_.first) maxNegDiffHit_.second+=hit.second;
      if(hit.first==maxPosDiffHit_.first) maxPosDiffHit_.second+=hit.second;
	
      double diff = diffFunc_(point);
      if(diff<=0 && diff<maxNegDiff_ && hit.first!=maxNegDiffHit_.first){
	maxNegDiff_ = diff;
	maxNegDiffHit_=hit;
      }
      if(diff>=0 && diff>maxPosDiff_ && hit.first!=maxPosDiffHit_.first){
	maxPosDiff_ =diff;
	maxPosDiffHit_ =hit;
      }
    }
    friend std::ostream& operator<<(std::ostream& out,const MaxDiffHitFinder& obj){return  obj.print(out);}
    std::ostream& print(std::ostream& out)const{
      out<<" max diff +ve "<<maxPosDiff_<<" hit : "<<maxPosDiffHit_.first.rawId()<<" frac "<<maxPosDiffHit_.second<<", max diff -ve "<<maxNegDiff_<<" hit : "<<maxNegDiffHit_.first.rawId()<<" frac "<<maxNegDiffHit_.second;
      return out;
    }
  };
  
  struct HitDiffs {
    float dNeg,dPos,idNeg,fracNeg,idPos,fracPos;
    static std::string contents(){return "dNeg/F:dPos:idNeg:fracNeg:idPos:fracPos";}
    void fill(const MaxDiffHitFinder& diff){
      dNeg = diff.maxNegDiff();
      dPos = diff.maxPosDiff();
      idNeg = diff.maxNegDiffHit().first.rawId();
      fracNeg = diff.maxNegDiffHit().second;
      idPos = diff.maxPosDiffHit().first.rawId();
      fracPos = diff.maxPosDiffHit().second;
    }
    void clear(){dNeg=dPos=fracNeg=fracPos=-999.;idPos=idNeg=0.;}
  };
  struct SeedTreeData {
    trigtools::EvtInfoStruct evtInfo; 
    trigtools::P4Struct p4Unseeded,p4Seeded,p4L1;
    float dRL1;
    HitDiffs etaHitDiffSC,phiHitDiffSC,etaHitDiffL1,phiHitDiffL1;

    void createBranches(TTree* tree){
      tree->Branch("evt",&evtInfo,trigtools::EvtInfoStruct::contents().c_str());
      tree->Branch("p4Unseeded",&p4Unseeded,trigtools::P4Struct::contents().c_str());
      tree->Branch("p4Seeded",&p4Seeded,trigtools::P4Struct::contents().c_str());
      tree->Branch("p4L1",&p4L1,trigtools::P4Struct::contents().c_str());
      tree->Branch("dRL1",&dRL1,"dRL1/F");
      tree->Branch("etaHitDiffSC",&etaHitDiffSC,HitDiffs::contents().c_str());
      tree->Branch("phiHitDiffSC",&phiHitDiffSC,HitDiffs::contents().c_str());
      tree->Branch("etaHitDiffL1",&etaHitDiffL1,HitDiffs::contents().c_str());
      tree->Branch("phiHitDiffL1",&phiHitDiffL1,HitDiffs::contents().c_str());   
    }
    void clearNonEvt(){
      p4Unseeded.clear();
      p4Seeded.clear();
      p4L1.clear();
      dRL1=-999;
      etaHitDiffSC.clear();
      phiHitDiffSC.clear();
      etaHitDiffL1.clear();
      phiHitDiffL1.clear();
    }
  };

    

private:
  

  edm::InputTag l1EGTag_;
  
  CandToken seededCandToken_,unseededCandToken_;
  edm::EDGetTokenT<l1t::EGammaBxCollection> l1EGToken_; 
  edm::EDGetTokenT<reco::PFClusterCollection> pfClustersToken_; 
  
  edm::ESHandle<CaloGeometry> calGeometryHandle_;
  
  SeedTreeData treeData_;
  TTree* tree_;

public:
  explicit SCSizeTreeMaker(const edm::ParameterSet& iPara);
  ~SCSizeTreeMaker(){}
  
 private:
  virtual void beginJob() override;
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  void analyzeSC(const reco::SuperCluster& superClus,
		 const reco::PFClusterCollection& pfClusters,
		 const math::XYZTLorentzVector& l1Pos);
  std::pair<const l1t::EGamma*,double> matchL1(const math::XYZTLorentzVector& cand,const l1t::EGammaBxCollection& egs,const double minL1Et,const double maxDR);
  std::pair<const math::XYZTLorentzVector*,double> matchHLT(const math::XYZTLorentzVector& cand,const std::vector<math::XYZTLorentzVector>& hltObjs,const double maxDR);
  std::pair<const reco::RecoEcalCandidate*,double> matchHLT(const reco::RecoEcalCandidateRef& cand,const std::vector<reco::RecoEcalCandidateRef>& hltObjs,const double maxDR);

  const reco::RecoEcalCandidate* matchSCBySeed(const reco::SuperCluster& sc,const std::vector<reco::RecoEcalCandidate>& cands);

  template <typename T> void setToken(edm::EDGetTokenT<T>& token,edm::InputTag tag){token=consumes<T>(tag);}
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tag){token=consumes<T>(iPara.getParameter<edm::InputTag>(tag));}
  
  void printSuperClusters(const std::vector<reco::RecoEcalCandidate>& seededCands,
			  const std::vector<reco::RecoEcalCandidate>& unseededCands,
			  const l1t::EGammaBxCollection& l1EGs);
  std::pair<float,float> getMaxEtaPhiRecHitDist(const reco::RecoEcalCandidate& cand,
						const l1t::EGammaBxCollection& egs);
  void printMaxRecHitDistsOfCands(const std::vector<reco::RecoEcalCandidate>& cands,
				  const l1t::EGammaBxCollection& l1EGs);
    
  std::vector<std::pair<DetId,float> > getAllHitsInInfluence(const reco::SuperCluster& superClus,const reco::PFClusterCollection& clusters);
  std::vector<const reco::PFCluster*> getClustersWithHit(DetId id,const reco::PFClusterCollection& clusters);
};



SCSizeTreeMaker::SCSizeTreeMaker(const edm::ParameterSet& iPara)
{

  setToken(l1EGToken_,iPara,"l1EGTag");
  setToken(seededCandToken_,iPara,"seededCands");
  setToken(unseededCandToken_,iPara,"unseededCands");
  setToken(pfClustersToken_,iPara,"pfClusters");
  
}

void SCSizeTreeMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_= new TTree("scSizeTree","");
  treeData_.createBranches(tree_);
  
}
void SCSizeTreeMaker::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
 

}
template<typename T> edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token)
{
  edm::Handle<T> handle;
  iEvent.getByToken(token,handle);
  return handle;
}


void SCSizeTreeMaker::analyzeSC(const reco::SuperCluster& superClus,
				const reco::PFClusterCollection& pfClusters,
				const math::XYZTLorentzVector& l1Pos)
				
{
  float l1Eta = l1Pos.eta();
  float l1Phi = l1Pos.phi();
  float scEta = superClus.eta();
  float scPhi = superClus.phi();

  MaxDiffHitFinder diffFromL1Eta([l1Eta](const GlobalPoint& point){
      return point.eta()-l1Eta;});
  MaxDiffHitFinder diffFromL1Phi([l1Phi](const GlobalPoint& point){
      return reco::deltaPhi(point.phi(),l1Phi);});
  MaxDiffHitFinder diffFromSCEta([scEta](const GlobalPoint& point){
      return point.eta()-scEta;});
  MaxDiffHitFinder diffFromSCPhi([scPhi](const GlobalPoint& point){
      return reco::deltaPhi(point.phi(),scPhi);});
 
  //for(auto subClus = superClus.clustersBegin();subClus!=superClus.clustersEnd();++subClus){
  //   for(auto& hit : (*subClus)->hitsAndFractions()){

  auto hits = getAllHitsInInfluence(superClus,pfClusters);
  for(auto& hit : hits){
    const CaloSubdetectorGeometry* subDetGeom =  calGeometryHandle_->getSubdetectorGeometry(hit.first);
    const CaloCellGeometry* cellGeom = subDetGeom ? subDetGeom->getGeometry(hit.first) : nullptr;
    if(cellGeom){
      const GlobalPoint& cellPos =cellGeom->getPosition();
      
      diffFromL1Eta(hit,cellPos);
      diffFromL1Phi(hit,cellPos);
      diffFromSCEta(hit,cellPos);
      diffFromSCPhi(hit,cellPos);
    }
  }
  treeData_.etaHitDiffSC.fill(diffFromSCEta);
  treeData_.phiHitDiffSC.fill(diffFromSCPhi);
  treeData_.etaHitDiffL1.fill(diffFromL1Eta);
  treeData_.phiHitDiffL1.fill(diffFromL1Phi);
  //std::cout <<"SC "<<superClus.energy()<<" "<<superClus.eta()<<" "<<superClus.phi()<<std::endl;
  //std::cout <<"   from L1 Eta "<<diffFromL1Eta<<std::endl;
  //std::cout <<"   from L1 Phi "<<diffFromL1Phi<<std::endl;
  //std::cout <<"   from SC Eta "<<diffFromSCEta<<std::endl;
  //std::cout <<"   from SC Phi "<<diffFromSCPhi<<std::endl;

}

std::vector<const reco::PFCluster*>  SCSizeTreeMaker::
getClustersWithHit(DetId id,const reco::PFClusterCollection& clusters)
{
  std::vector<const reco::PFCluster*> retVal;
  for(auto& clus : clusters){
    for(auto & hit : clus.hitsAndFractions()){
      if(hit.first==id){
	retVal.push_back(&clus);
	break;
      }
    }
  }
  return retVal;
}
struct SetComp{
  
  bool operator()(const reco::CaloCluster* lhs,const reco::CaloCluster* rhs){return lhs->seed().rawId()<rhs->seed().rawId();}
};

std::vector<std::pair<DetId,float> > SCSizeTreeMaker::
getAllHitsInInfluence(const reco::SuperCluster& superClus,const reco::PFClusterCollection& clusters)
{
  
  std::set<const reco::CaloCluster*,SetComp> influencingClusters;
  for(auto subClus = superClus.clustersBegin();subClus!=superClus.clustersEnd();++subClus){
    influencingClusters.insert(&**subClus);
    for(auto& hit : (*subClus)->hitsAndFractions()){
      if(hit.second!=1){ //another cluster also shares this hit...
	std::vector<const reco::PFCluster*> sharingClusters = getClustersWithHit(hit.first,clusters);
	for(auto clus : sharingClusters) influencingClusters.insert(clus);
      }
    }
  }
  
  std::vector<std::pair<DetId,float> > retVal;
  //  std::cout <<"start clusters "<<std::endl;
  for(auto clus : influencingClusters){
    //std::cout <<"clus "<<clus<<" "<<clus->energy()<<" eta "<<clus->eta()<<" phi "<<clus->phi()<<std::endl;
    for(auto& hit : clus->hitsAndFractions()) retVal.push_back(hit);
  }
  return retVal;

}

void SCSizeTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  iSetup.get<CaloGeometryRecord>().get(calGeometryHandle_);


  auto l1EGs = getHandle(iEvent,l1EGToken_);
  auto seededCands = getHandle(iEvent,seededCandToken_);
  auto unseededCands = getHandle(iEvent,unseededCandToken_);
  auto pfClusters = getHandle(iEvent,pfClustersToken_);
  

  treeData_.evtInfo.fill(iEvent);
  for(auto& seededCand : *seededCands){
    const reco::RecoEcalCandidate* matchedCand = matchSCBySeed(*seededCand.superCluster(),*unseededCands);
    if(matchedCand==nullptr){
      std::cout <<"Error seeded cluster does not have a matching seeded cluster, this was thought to be impossible " <<iEvent.id().run()<<" "<<iEvent.luminosityBlock()<<" "<<iEvent.id().event()<<std::endl;
      
      std::cout <<"   et "<<seededCand.et()<<" eta "<<seededCand.eta()<<" phi "<<seededCand.phi()<<std::endl;
      for(auto& unseededCand : *unseededCands){
	std::cout <<"       et "<<unseededCand.et()<<" eta "<<unseededCand.eta()<<" phi "<<unseededCand.phi()<<std::endl;
      }
    }
    auto matchedEG = matchL1(seededCand.p4(),*l1EGs,0,0.2);
    if(matchedEG.first==nullptr){
      //      std::cout <<"Error seeded cluster does not have a matching L1 object, this was thought to be impossible "<<iEvent.id().run()<<" "<<iEvent.luminosityBlock()<<" "<<iEvent.id().event()<<std::endl;
    }
    treeData_.clearNonEvt();
    
    treeData_.p4Seeded.fill(seededCand.p4());
    if(matchedEG.first){
      treeData_.dRL1 = matchedEG.second;
      treeData_.p4L1.fill(matchedEG.first->p4());
    }
    if(matchedCand){
      treeData_.p4Unseeded.fill(matchedCand->p4());
    }
    if(matchedCand && matchedEG.first){
      analyzeSC(*matchedCand->superCluster(),*pfClusters,matchedEG.first->p4());
    }
    tree_->Fill();
    
  }
  

}



void SCSizeTreeMaker::
printMaxRecHitDistsOfCands(const std::vector<reco::RecoEcalCandidate>& cands,
			   const l1t::EGammaBxCollection& l1EGs)
{
  std::cout <<"printing max dEta,dPhi"<<std::endl;
  for(auto& cand : cands){
    
    getMaxEtaPhiRecHitDist(cand,l1EGs);
  }
}

std::pair<const math::XYZTLorentzVector*,double> SCSizeTreeMaker::matchHLT(const math::XYZTLorentzVector& cand,const std::vector<math::XYZTLorentzVector>& hltObjs,const double maxDR)
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
SCSizeTreeMaker::matchSCBySeed(const reco::SuperCluster& sc,const std::vector<reco::RecoEcalCandidate>& cands)
{
  for(auto& cand : cands){
    if(cand.superCluster()->seed()->seed()==sc.seed()->seed()) return &cand;
  }
  //well that didnt work, lets see if our subclusters got yanked into another supercluster
  for(auto& cand : cands){
    for(auto scBC = sc.clustersBegin();scBC!=sc.clustersEnd();++scBC){
      for(auto candBC = cand.superCluster()->clustersBegin();candBC!=cand.superCluster()->clustersEnd();++candBC){
	if((*scBC)->seed()==(*candBC)->seed()){
	  return &cand;
	}
      }
    }
  }
  //really?
  return nullptr;
    
}

std::pair<const reco::RecoEcalCandidate*,double> 
SCSizeTreeMaker::matchHLT(const reco::RecoEcalCandidateRef& cand,const std::vector<reco::RecoEcalCandidateRef>& hltObjs,const double maxDR)
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


void SCSizeTreeMaker::
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
SCSizeTreeMaker::
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


std::pair<const l1t::EGamma*,double> SCSizeTreeMaker::matchL1(const math::XYZTLorentzVector& cand,const l1t::EGammaBxCollection& egs,const double minL1Et,const double maxDR)
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
DEFINE_FWK_MODULE(SCSizeTreeMaker);
