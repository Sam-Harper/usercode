#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"
class HEEPIdAndVIDComp : public edm::EDAnalyzer {

private:
 
  edm::EDGetTokenT<edm::View<reco::GsfElectron> > eleToken_;
  edm::EDGetTokenT<edm::ValueMap<int> > heepIdToken_;
  edm::EDGetTokenT<edm::ValueMap<bool> > vidToken_;

public:
  explicit HEEPIdAndVIDComp(const edm::ParameterSet& iPara);
  virtual ~HEEPIdAndVIDComp(){}
  
private:
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup) override;
 
};
  

HEEPIdAndVIDComp::HEEPIdAndVIDComp(const edm::ParameterSet& iPara)
{
  eleToken_=consumes<edm::View<reco::GsfElectron> >(iPara.getParameter<edm::InputTag>("eleLabel"));
  heepIdToken_=consumes<edm::ValueMap<int> >(iPara.getParameter<edm::InputTag>("heepId"));
  vidToken_=consumes<edm::ValueMap<bool> >(iPara.getParameter<edm::InputTag>("vid"));
  
}


void HEEPIdAndVIDComp::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<reco::GsfElectron> > eleHandle;
  edm::Handle<edm::ValueMap<int> > heepId;
  edm::Handle<edm::ValueMap<bool> > vid;

  iEvent.getByToken(eleToken_,eleHandle);
  iEvent.getByToken(heepIdToken_,heepId);
  iEvent.getByToken(vidToken_,vid);
  
  std::cout <<"running over all the electrons"<<std::endl;
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){  
    std::cout<<"electron et "<<(*eleHandle)[eleNr].p4().px()<<std::endl;

    edm::Ptr<reco::GsfElectron> elePtr(eleHandle,eleNr);
    bool passVid=(*vid)[elePtr];
    int heepBits =(*heepId)[elePtr];
    bool passHEEP=heepBits==0x0;
    if(passVid!=passHEEP){
      std::cout <<"VID - HEEP disagreement, vid "<<passVid<<" heep "<<heep::CutCodes::getCodeName(heepBits)<<std::endl;
    }
  }

  for(auto& ele : *eleHandle){  
    const_cast<math::XYZTLorentzVector&>(ele.p4()).SetPx(35.5);
  }
}
DEFINE_FWK_MODULE(HEEPIdAndVIDComp);
