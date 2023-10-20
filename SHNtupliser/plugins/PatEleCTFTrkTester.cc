
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
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

class PatEleCTFTrkTester : public edm::one::EDAnalyzer<> {

private:
  edm::EDGetTokenT<edm::View<pat::Electron> > patElesToken_;
  edm::EDGetTokenT<std::vector<pat::PackedCandidate> > packedPFCandsToken_;
 
public:
  explicit PatEleCTFTrkTester(const edm::ParameterSet& para);
  ~PatEleCTFTrkTester(){}
  
  virtual void beginJob();
  virtual void analyze(const edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();


  static bool passCuts(const reco::Track& trk);
  
  static const pat::Electron* getMatchedEle(const pat::PackedCandidate& cand,
					    const edm::View<pat::Electron>& patEles);

private:
 
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

PatEleCTFTrkTester::PatEleCTFTrkTester(const edm::ParameterSet& para)
{
  getToken_(packedPFCandsToken_,para,"packedPFCandsTag");
  getToken_(patElesToken_,para,"patElesTag");
}

void PatEleCTFTrkTester::beginJob()
{
  
}

void PatEleCTFTrkTester::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  
  auto pfCands = getHandle_(iEvent,packedPFCandsToken_);
  auto patEles = getHandle_(iEvent,patElesToken_);
  
  for(auto& pfCand : *pfCands){


    if(std::abs(pfCand.pdgId())==11){
      auto matchedEle = getMatchedEle(pfCand,*patEles);
      if(matchedEle){
	auto trkRef = matchedEle->closestCtfTrackRef();
	if(trkRef.isNull()){
	  auto pseudoTrk = pfCand.pseudoTrack();
	  std::cout <<" cand "<<pfCand.pt()<<" "<<pfCand.eta()<<" "<<pfCand.phi()<<" "<<pfCand.pdgId()<<" does not have stored track "<<pseudoTrk.pt()<<" "<<pseudoTrk.numberOfValidHits()<<" "<<pseudoTrk.hitPattern().numberOfValidPixelHits()<<std::endl;
	}else{
	  std::cout <<" cand "<<pfCand.pt()<<" "<<pfCand.eta()<<" "<<pfCand.phi()<<" "<<pfCand.pdgId()<<" has stored track"<<std::endl;
	}
      }else{
	std::cout <<" cand "<<pfCand.pt()<<" "<<pfCand.eta()<<" "<<pfCand.phi()<<" "<<pfCand.pdgId()<<" does not have PF Cand"<<std::endl;
		  
      }
	
    }
  }
}


void PatEleCTFTrkTester::endJob()
{ 
 
  
}  







const pat::Electron*
PatEleCTFTrkTester::
getMatchedEle(const pat::PackedCandidate& cand,const edm::View<pat::Electron>& patEles)
{
  auto isEqual=[](float lhs,float rhs){return std::abs(lhs-rhs)<0.001;};
  for(auto& ele : patEles){
    if(isEqual(cand.eta(),ele.eta()) && isEqual(cand.phi(),ele.phi())) return &ele;
  }
  return nullptr;
}
 

bool 
PatEleCTFTrkTester::
passCuts(const reco::Track& trk)
{
  if(trk.pt()>1 &&
     trk.numberOfValidHits()>=8 &&
     trk.hitPattern().numberOfValidPixelHits()>=1) return true;
  else return false;
}



DEFINE_FWK_MODULE(PatEleCTFTrkTester);
