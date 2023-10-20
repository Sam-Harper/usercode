

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

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"


#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/Common/interface/AssociationMap.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"

#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/EgammaReco/interface/ElectronSeedFwd.h"

#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "TTree.h"

#include <vector>
#include <string>


 

class EgHLTTrkComp : public edm::one::EDAnalyzer<> {
public:
  using RecoEcalCandMap = edm::AssociationMap<edm::OneToValue<std::vector<reco::RecoEcalCandidate>, float > >;
  using CandSeedVec =  std::vector<std::pair<reco::RecoEcalCandidateRef,const reco::ElectronSeed*> >;
 
private:
  
  TTree* tree_;
  trigtools::EvtInfoStruct evtInfo_; 
  trigtools::P4Struct track1P4_,track2P4_,sc1P4_,sc2P4_;
  float var1_;
  float var2_;
  int trkNr_;

  std::string treeName_;

  edm::EDGetTokenT<reco::GsfTrackCollection> track1Token_,track2Token_;
  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> candToken_;
  edm::EDGetTokenT<RecoEcalCandMap> varToken_;
  
  
public:
  explicit EgHLTTrkComp(const edm::ParameterSet& iPara);
  ~EgHLTTrkComp(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,edm::InputTag tag){token=consumes<T>(tag);}
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tag){token=consumes<T>(iPara.getParameter<edm::InputTag>(tag));}
  			
  
  void clearTrkData();

};



EgHLTTrkComp::EgHLTTrkComp(const edm::ParameterSet& iPara)
{
  treeName_ = iPara.getParameter<std::string>("treeName");
  setToken(candToken_,iPara,"cands");
  setToken(varToken_,iPara,"var");
  setToken(track1Token_,iPara,"tracks1");  
  setToken(track2Token_,iPara,"tracks2");  

}

void EgHLTTrkComp::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_= new TTree(treeName_.c_str(),"hlt trigger");
  tree_->Branch("evt",&evtInfo_,trigtools::EvtInfoStruct::contents().c_str());
  tree_->Branch("trk1P4",&track1P4_,trigtools::P4Struct::contents().c_str());
  tree_->Branch("trk2P4",&track2P4_,trigtools::P4Struct::contents().c_str());
  tree_->Branch("sc1P4",&sc1P4_,trigtools::P4Struct::contents().c_str());
  tree_->Branch("sc2P4",&sc2P4_,trigtools::P4Struct::contents().c_str());
  tree_->Branch("var1",&var1_,"var1/F");
  tree_->Branch("var2",&var2_,"var2/F");
  tree_->Branch("trkNr",&trkNr_,"trkNr/I");


}

std::vector<std::pair<const reco::GsfTrack*,const reco::GsfTrack*> >
matchTracks(const reco::GsfTrackCollection& trks1,const reco::GsfTrackCollection& trks2)
{
  std::vector<std::pair<const reco::GsfTrack*,const reco::GsfTrack*> > matchedTrks;
  for(auto& trk1 : trks1){
    auto matchFunc=[trk1](const reco::GsfTrack& trk2){
      return reco::deltaR2(trk1.eta(),trk1.phi(),trk2.eta(),trk2.phi())<0.001*0.001;
    };
    auto matchedTrk = std::find_if(trks2.begin(),trks2.end(),matchFunc);
    if(matchedTrk!=trks2.end()) matchedTrks.push_back({&trk1,&*matchedTrk});
    else matchedTrks.push_back({&trk1,nullptr});
  }
  for(auto& trk2 : trks2){
    auto matchFunc=[trk2](const reco::GsfTrack& trk1){
      return reco::deltaR2(trk1.eta(),trk1.phi(),trk2.eta(),trk2.phi())<0.001*0.001;
    };
    if(std::find_if(trks1.begin(),trks1.end(),matchFunc)==trks1.end()){
      matchedTrks.push_back({nullptr,&trk2});
    }
  }
  return matchedTrks;

}

template<typename T> edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token)
{
  edm::Handle<T> handle;
  iEvent.getByToken(token,handle);
  return handle;
}
  

		
float getVal(reco::RecoEcalCandidateRef cand,
	     edm::Handle<EgHLTTrkComp::RecoEcalCandMap> values)
{
  auto valIt = values->find(cand);
  if(valIt!=values->end()) return valIt->val;
  else return -1;
}
	
 		      


reco::SuperClusterRef getSCRef(const reco::GsfTrack& track)
{
  edm::RefToBase<TrajectorySeed> seed = track.extra()->seedRef() ;
  reco::ElectronSeedRef elseed = seed.castTo<reco::ElectronSeedRef>() ;
  edm::RefToBase<reco::CaloCluster> caloCluster = elseed->caloCluster() ;
  reco::SuperClusterRef scRefFromTrk = caloCluster.castTo<reco::SuperClusterRef>() ;
  return scRefFromTrk;
}

math::XYZTLorentzVector getTrkP4(const reco::Track& trk)
{
  math::XYZTLorentzVector p4(trk.px(),trk.py(),trk.pz(),trk.p());
  return p4;
}

math::XYZTLorentzVector getSCP4(const reco::SuperCluster& sc)
{
  math::XYZTLorentzVector p4(sc.position().x(),sc.position().y(),sc.position().z(),sc.energy());
  return p4;
}

void EgHLTTrkComp::clearTrkData()
{
  track1P4_.clear();
  track2P4_.clear();
  sc1P4_.clear();
  sc2P4_.clear();
  var1_=-999;
  var2_=-999;
}

float getVar(const reco::SuperClusterRef sc,edm::Handle<reco::RecoEcalCandidateCollection>& cands,
	     edm::Handle<EgHLTTrkComp::RecoEcalCandMap> varVals)
{
  for(size_t candNr=0;candNr<cands->size();candNr++){
    auto candRef = edm::Ref<reco::RecoEcalCandidateCollection>(cands,candNr);
    if(candRef->superCluster()==sc){
      return getVal(candRef,varVals);
    }
  }
  return -9999;
}

void EgHLTTrkComp::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  auto cands = getHandle(iEvent,candToken_);
  auto tracks1 = getHandle(iEvent,track1Token_);
  auto tracks2 = getHandle(iEvent,track2Token_);  
  auto varValues = getHandle(iEvent,varToken_);

  auto matchedTrks = matchTracks(*tracks1,*tracks2);
  
 
  
  evtInfo_.fill(iEvent);
  for(size_t trkNr=0;trkNr<matchedTrks.size();trkNr++){
    clearTrkData();
    auto& trkPair = matchedTrks[trkNr];
    trkNr_=trkNr;
    if(trkPair.first){
      track1P4_.fill(getTrkP4(*trkPair.first));
      auto sc = getSCRef(*trkPair.first);
      sc1P4_.fill(getSCP4(*sc));
      var1_ = getVar(sc,cands,varValues);
    }
    if(trkPair.second){
      track2P4_.fill(getTrkP4(*trkPair.second));
      auto sc =getSCRef(*trkPair.second);
      sc2P4_.fill(getSCP4(*sc));
      var2_ = getVar(sc,cands,varValues);
    }
    tree_->Fill();
  }
  
}



//define this as a plug-in
DEFINE_FWK_MODULE(EgHLTTrkComp);
