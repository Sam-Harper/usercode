

#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"
#include "SHarper/TrigTools/interface/TrigToolsStructs.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TTree.h"

#include <vector>
#include <string>


 

class HLTZPeakMon : public edm::EDAnalyzer {
private:
  
  TTree* tree_;
  trigtools::EvtInfoStruct evtInfo_; 
  trigtools::P4Struct obj1P4_,obj2P4_;
  int pairNr_;
  int nrCands_;
  float mass_;

  std::string treeName_;
  std::string filterName_;
  edm::InputTag trigEvtTag_;
  edm::EDGetTokenT<trigger::TriggerEvent> trigEvtToken_;
  
  
public:
  explicit HLTZPeakMon(const edm::ParameterSet& iPara);
  ~HLTZPeakMon(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,edm::InputTag tag){token=consumes<T>(tag);}
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tag){token=consumes<T>(iPara.getParameter<edm::InputTag>(tag));}
  			

};


HLTZPeakMon::HLTZPeakMon(const edm::ParameterSet& iPara)
{
  treeName_ = iPara.getParameter<std::string>("treeName");
  filterName_ = iPara.getParameter<std::string>("filterName");
  trigEvtTag_ = iPara.getParameter<edm::InputTag>("trigEvent");
  setToken(trigEvtToken_,iPara,"trigEvent");
}

void HLTZPeakMon::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_= new TTree(treeName_.c_str(),"hlt trigger");
  tree_->Branch("evt",&evtInfo_,trigtools::EvtInfoStruct::contents().c_str());
  tree_->Branch("obj1P4",&obj1P4_,trigtools::P4Struct::contents().c_str());
  tree_->Branch("obj2P4",&obj2P4_,trigtools::P4Struct::contents().c_str());
  tree_->Branch("mass",&mass_,"mass/F");
  tree_->Branch("nrCands",&nrCands_,"nrCands/I");
  tree_->Branch("pairNr",&pairNr_,"pairNr/I");
}

template<typename T> edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token)
{
  edm::Handle<T> handle;
  iEvent.getByToken(token,handle);
  return handle;
}
  

void HLTZPeakMon::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  auto trigEvt = getHandle(iEvent,trigEvtToken_);
  std::vector<math::XYZTLorentzVector> trigs;
  trigtools::getP4sOfObsPassingFilter(trigs,*trigEvt,filterName_,trigEvtTag_.process());
  


  std::sort(trigs.begin(),trigs.end(),
	    [](const math::XYZTLorentzVector& lhs,const math::XYZTLorentzVector& rhs){
	      return lhs.pt() > rhs.pt();
	    });

  evtInfo_.fill(iEvent);
  pairNr_=0;
  nrCands_=trigs.size();
  for(size_t trig1Nr=0;trig1Nr<trigs.size();trig1Nr++){
    for(size_t trig2Nr=trig1Nr+1;trig2Nr<trigs.size();trig2Nr++){      
      obj1P4_.fill(trigs[trig1Nr]);
      obj2P4_.fill(trigs[trig2Nr]);
      mass_ = (trigs[trig1Nr]+trigs[trig2Nr]).mag();
      tree_->Fill();
      pairNr_++;
    }
  }
}



//define this as a plug-in
DEFINE_FWK_MODULE(HLTZPeakMon);
