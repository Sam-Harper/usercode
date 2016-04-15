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
  edm::EDGetTokenT<double>  eleRhoCorrToken_;
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
  eleRhoCorrToken_=consumes<double>(iPara.getParameter<edm::InputTag>("rho"));
}

bool heepTrkPtCut(const reco::GsfElectron& ele,double rho)
{
  const float startEtEB = 95.;
  const float startEtEE = 100.;
  const float rhoEAEB=1.5;
  const float rhoEAEE=0.5;
  
  const bool isEB = std::abs(ele.superCluster()->eta())<1.5;
  const float startEt = isEB ? startEtEB :startEtEE;
  const float rhoEA = isEB ? rhoEAEB : rhoEAEE;

  const float rhoCorr = ele.et()>=startEt ? rhoEA*rho : 0.;

  return ele.dr03TkSumPt()< 5 + rhoCorr;

}


void HEEPIdAndVIDComp::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<reco::GsfElectron> > eleHandle;
  edm::Handle<edm::ValueMap<int> > heepId;
  edm::Handle<edm::ValueMap<bool> > vid;

  iEvent.getByToken(eleToken_,eleHandle);
  iEvent.getByToken(heepIdToken_,heepId);
  iEvent.getByToken(vidToken_,vid);

  edm::Handle<double> rhoHandle;
  iEvent.getByToken(eleRhoCorrToken_,rhoHandle);
  double rho = *rhoHandle;// rhoHandle.isValid() ? *rhoHandle : 0;

  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){  
    edm::Ptr<reco::GsfElectron> elePtr(eleHandle,eleNr);
    bool passVid=(*vid)[elePtr];
    int heepBits =(*heepId)[elePtr];
    bool passHEEP=(heepBits&(~heep::CutCodes::ISOLPTTRKS))==0x0;// && heepTrkPtCut(*elePtr,rho);
    bool passHEEPTrk = heepTrkPtCut(*elePtr,rho);
    if(passVid!=(passHEEP && passHEEPTrk)){
      std::cout <<"VID - HEEP disagreement, vid "<<passVid<<" heep "<<heep::CutCodes::getCodeName(heepBits)<<" heepIsolTrk "<<passHEEPTrk<<" ele et "<<elePtr->et()<<" eta "<<elePtr->eta()<<std::endl;
      
      std::cout <<"     trk isol "<<elePtr->dr03TkSumPt()<<" rho "<<rho<<std::endl;
    }
  }
}
DEFINE_FWK_MODULE(HEEPIdAndVIDComp);
