
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "TTree.h"

class AODVsMiniAODTracksComp : public edm::EDAnalyzer {

private:


  
  edm::EDGetTokenT<reco::TrackCollection> aodTrksToken_;
  edm::EDGetTokenT<edm::View<pat::Electron> > patElesToken_;
  edm::EDGetTokenT<std::vector<pat::PackedCandidate> > packedPFCandsToken_;
  edm::EDGetTokenT<std::vector<pat::PackedCandidate> > lostTrksToken_;
  edm::EDGetTokenT<std::vector<reco::PFCandidate> > pfCandsToken_;
 
public:
  explicit AODVsMiniAODTracksComp(const edm::ParameterSet& para);
  ~AODVsMiniAODTracksComp(){}
  
  virtual void beginJob();
  virtual void analyze(const edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();


  static bool passCuts(const reco::Track& trk);
  
  static bool isTrkInColl(const reco::Track& trk,
			  const std::vector<pat::PackedCandidate>& cands);
  
  static int getPFCandId(const reco::Track& trk,
			 const std::vector<reco::PFCandidate>& cands);
  static const reco::PFCandidate* getPFCand(const reco::Track& trk,
		       const std::vector<reco::PFCandidate>& cands);
  
  static const pat::Electron* getMatchedEle(const reco::PFCandidate* cand,
					    const edm::View<pat::Electron>& patEles);

  static bool isInGenTracks(const reco::Track* trk,const reco::TrackCollection& genTrks);
private:
  reco::TrackCollection filterAODTrks_(const reco::TrackCollection& trks);
  void compareTrks_(const edm::Event& event,
		    const reco::TrackCollection& aodTrks,
		    const std::vector<pat::PackedCandidate>& packedCands,
		    const std::vector<pat::PackedCandidate>& lostTrks);

  template<typename T>
  void getToken_(edm::EDGetTokenT<T>& token,const edm::InputTag& tag){
    token=consumes<T>(tag);
  }
  template <typename T>
  void getToken_(edm::EDGetTokenT<T>& token,const edm::ParameterSet& para,std::string&& name){
    getToken_(token,para.getParameter<edm::InputTag>(name));
  }
  template<typename T>
  edm::Handle<T> getHandle_(const edm::Event& event,edm::EDGetTokenT<T>& token){
    edm::Handle<T> handle;
    event.getByToken(token,handle);
    return handle;
  }
    
 
};

AODVsMiniAODTracksComp::AODVsMiniAODTracksComp(const edm::ParameterSet& para)
{
  getToken_(aodTrksToken_,para,"aodTrksTag");
  getToken_(patElesToken_,para,"patElesTag");
  getToken_(packedPFCandsToken_,para,"packedPFCandsTag");
  getToken_(lostTrksToken_,para,"lostTrksTag");  
  getToken_(pfCandsToken_,para,"pfCandsTag");
}

void AODVsMiniAODTracksComp::beginJob()
{
  
}

void AODVsMiniAODTracksComp::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  auto aodTrks = getHandle_(iEvent,aodTrksToken_);
  auto packedPFCands = getHandle_(iEvent,packedPFCandsToken_);
  auto lostTrks = getHandle_(iEvent,lostTrksToken_);

  auto filteredAODTrks  = filterAODTrks_(*aodTrks);
  
  compareTrks_(iEvent,filteredAODTrks,*packedPFCands,*lostTrks);

}

void AODVsMiniAODTracksComp::endJob()
{ 
 
  
}  

bool 
AODVsMiniAODTracksComp::
isTrkInColl(const reco::Track& trk,
	    const std::vector<pat::PackedCandidate>& cands)
{
  auto isEqual=[](float lhs,float rhs){return std::abs(lhs-rhs)<0.001;};

  for(auto& cand : cands){
    auto& candTrk = cand.pseudoTrack();
    if(isEqual(trk.eta(),candTrk.eta()) && isEqual(trk.phi(),candTrk.phi())) return true;
    
  }
  return false;
}




int
AODVsMiniAODTracksComp::
getPFCandId(const reco::Track& trk,
	       const std::vector<reco::PFCandidate>& cands)
{
  auto isEqual=[](float lhs,float rhs){return std::abs(lhs-rhs)<0.001;};

  for(auto& cand : cands){
    if(cand.trackRef().isNonnull()){
      auto& candTrk = *cand.trackRef();
      if(isEqual(trk.eta(),candTrk.eta()) && isEqual(trk.phi(),candTrk.phi())) return cand.pdgId();
    }
  }
  return 0;
}

const reco::PFCandidate*
AODVsMiniAODTracksComp::
getPFCand(const reco::Track& trk,
	  const std::vector<reco::PFCandidate>& cands)
{
  auto isEqual=[](float lhs,float rhs){return std::abs(lhs-rhs)<0.001;};

  for(auto& cand : cands){
    if(cand.trackRef().isNonnull()){
      auto& candTrk = *cand.trackRef();
      if(isEqual(trk.eta(),candTrk.eta()) && isEqual(trk.phi(),candTrk.phi())) return &cand;
    }
  }
  return nullptr;
}

const pat::Electron*
AODVsMiniAODTracksComp::
getMatchedEle(const reco::PFCandidate* cand,const edm::View<pat::Electron>& patEles)
{
  auto isEqual=[](float lhs,float rhs){return std::abs(lhs-rhs)<0.001;};
  for(auto& ele : patEles){
    if(isEqual(cand->eta(),ele.eta()) && isEqual(cand->phi(),ele.phi())) return &ele;
  }
  return nullptr;
}

reco::TrackCollection 
AODVsMiniAODTracksComp::
filterAODTrks_(const reco::TrackCollection& trks)
{
  reco::TrackCollection retVal;
  
  for(auto& trk : trks){
    if(passCuts(trk)) retVal.push_back(trk);
  }
  return retVal;
}

void 
AODVsMiniAODTracksComp::
compareTrks_(const edm::Event& event,
	     const reco::TrackCollection& aodTrks,
	     const std::vector<pat::PackedCandidate>& packedCands,
	     const std::vector<pat::PackedCandidate>& lostTrks)
{
  auto pfCands = getHandle_(event,pfCandsToken_);
  auto patEles = getHandle_(event,patElesToken_);
  
  auto allAODTrks = getHandle_(event,aodTrksToken_);
  std::vector<const reco::Track*> missingTrks;
  bool printedEvent=false;

  for(auto& trk : aodTrks){
    bool isInPackedCands = isTrkInColl(trk,packedCands);
    bool isInLostTrks = isTrkInColl(trk,lostTrks);
    int pfCandId = 0;
    auto matchedPFCand = getPFCand(trk,*pfCands);
    if(matchedPFCand) pfCandId = matchedPFCand->pdgId();
    if(matchedPFCand){  
      auto isEqual=[](float lhs,float rhs){return std::abs(lhs-rhs)<0.001;};
      if(std::abs(pfCandId)!=11 
	 //&& trk.ptError()/trk.pt()<0.1
	 && !isEqual(trk.pt(),matchedPFCand->pt())
	 && false){
	if(!printedEvent){
	  std::cout <<"Event "<<event.id().run()<<" "<<event.luminosityBlock()<<" "<<event.id().event()<<
	    " has bad pt match"<<std::endl;
	  printedEvent=true;
	}
	std::cout <<" bad pt match "<<trk.pt()<<" "<<trk.ptError()/trk.pt()<<" "<<matchedPFCand->pt()<<" "<<pfCandId<<std::endl;
      }
    }
    if(std::abs(pfCandId)==11){
      auto matchedEle = getMatchedEle(matchedPFCand,*patEles);
      if(matchedEle){
	if(matchedEle->gsfTrack().isNull()){
	  std::cout <<" cand "<<matchedPFCand->pt()<<" "<<matchedPFCand->eta()<<" "<<matchedPFCand->phi()<<" does not have stored GSF track"<<std::endl;
	  std::cout <<"      trk "<<trk.pt()<<" "<<trk.eta()<<" "<<trk.phi()<<std::endl;
	  std::cout <<"      ele "<<matchedEle->pt()<<" "<<matchedEle->eta()<<" "<<matchedEle->phi()<<std::endl;
	  std::cout <<"      candGsf "<<matchedPFCand->gsfTrackRef()->pt()<<" "<<matchedPFCand->gsfTrackRef()->eta()<<" "<<matchedPFCand->gsfTrackRef()->phi()<<std::endl; 
	  std::cout <<"      candCtf "<<matchedPFCand->trackRef()->pt()<<" "<<matchedPFCand->trackRef()->eta()<<" "<<matchedPFCand->trackRef()->phi()<<std::endl;
	}
	const auto& trkRef = matchedEle->closestCtfTrackRef();
	if(trkRef.isNull()){
	  std::cout <<" cand "<<matchedPFCand->pt()<<" "<<matchedPFCand->eta()<<" "<<matchedPFCand->phi()<<" does not have stored track "<< matchedEle->hadronicOverEm()<<" "<<matchedEle->passingCutBasedPreselection()<<" "<<matchedEle->passingPflowPreselection()<<std::endl;
	  std::cout <<"      trk "<<trk.pt()<<" "<<trk.eta()<<" "<<trk.phi()<<std::endl;
	  std::cout <<"      ele "<<matchedEle->pt()<<" "<<matchedEle->eta()<<" "<<matchedEle->phi()<<std::endl;
	  std::cout <<"      candGsf "<<matchedPFCand->gsfTrackRef()->pt()<<" "<<matchedPFCand->gsfTrackRef()->eta()<<" "<<matchedPFCand->gsfTrackRef()->phi()<<std::endl; 
	  std::cout <<"      candCtf "<<matchedPFCand->trackRef()->pt()<<" "<<matchedPFCand->trackRef()->eta()<<" "<<matchedPFCand->trackRef()->phi()<<std::endl;
	  
	}else{
	  //bool isInGenTrks = isInGenTracks(&*trkRef,*allAODTrks);
	  //  std::cout <<" cand "<<matchedPFCand->pt()<<" "<<matchedPFCand->eta()<<" "<<matchedPFCand->phi()<<" has stored track "<<trkRef.id()<<" "<<trkRef->pt()<<" "<<trkRef.key()<<" allAOD "<<isInGenTrks<<std::endl;
	}
      }else{
	std::cout <<" cand "<<matchedPFCand->pt()<<" "<<matchedPFCand->eta()<<" "<<matchedPFCand->phi()<<" doesnt not have PF Cand"<<std::endl;
		  
      }
	
    }
	 

    if(!isInPackedCands && !isInLostTrks && std::abs(pfCandId)!=11){
      missingTrks.push_back(&trk);
      
    }
  }
  if(!missingTrks.empty() && false){
    std::cout <<"Event "<<event.id().run()<<" "<<event.luminosityBlock()<<" "<<event.id().event()<<
      " has missing tracks"<<std::endl;
    for(auto trk : missingTrks){
      int pfCandId = getPFCandId(*trk,*pfCands);
      std::cout <<"   missing track candId "<<pfCandId<<" pt/eta/phi " <<trk->pt()<<" "<<trk->eta()<<" "<<trk->phi()<<" dPtPt "<<trk->ptError()/trk->pt()<<std::endl;
      auto matchedPFCand = getPFCand(*trk,*pfCands);
      std::cout <<"   missing trk pf cand pt/eta/phi ";
      if(matchedPFCand) std::cout <<matchedPFCand->pt()<<" "<<matchedPFCand->eta()<<" "<<matchedPFCand->phi()<<std::endl;
      else std::cout <<" null "<<std::endl;
      // std::cout <<"   Packed PF Cands "<<std::endl;
      // std::vector<pat::PackedCandidate> packedCandsSorted(packedCands);
      // auto sorter =[](const pat::PackedCandidate&lhs,const pat::PackedCandidate& rhs){return lhs.eta()<rhs.eta();};
      // std::sort(packedCandsSorted.begin(),packedCandsSorted.end(),sorter);
      // for(auto& cand : packedCandsSorted){
      // 	std::cout <<"         "<<cand.pt()<<" "<<cand.eta()<<" "<<cand.phi()<<" id "<<cand.pdgId()<<std::endl;
      // }
      // std::cout <<"   Lost Tracks "<<std::endl;
      // for(auto& cand : lostTrks){
      // 	std::cout <<"         "<<cand.pt()<<" "<<cand.eta()<<" "<<cand.phi()<<" id "<<cand.pdgId()<<std::endl;
      // }
      
    }
  }
}

bool 
AODVsMiniAODTracksComp::
isInGenTracks(const reco::Track* trk,const reco::TrackCollection& genTrks)
{
  for(auto& genTrk : genTrks){
    if(trk==&genTrk) return true;
  }
  return false;

}

bool 
AODVsMiniAODTracksComp::
passCuts(const reco::Track& trk)
{
  if(trk.pt()>1 &&
     trk.numberOfValidHits()>=8 &&
     trk.hitPattern().numberOfValidPixelHits()>=1) return true;
  else return false;
}



DEFINE_FWK_MODULE(AODVsMiniAODTracksComp);
