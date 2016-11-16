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
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


#include "TH1D.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

class HEEPV70Example : public edm::EDAnalyzer {

private:
 
  edm::EDGetTokenT<edm::View<reco::GsfElectron> > eleToken_;
  edm::EDGetTokenT<edm::ValueMap<bool> > vidToken_; //VID is versioned ID, is the standard E/gamma ID producer which we have configured for HEEP
  edm::EDGetTokenT<edm::ValueMap<float> > trkIsolMapToken_;

  
public:
  explicit HEEPV70Example(const edm::ParameterSet& iPara);
  virtual ~HEEPV70Example(){}
  
private:
  void beginJob() override;
  void endJob() override;
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup) override;
  const reco::GenParticle* matchGenPart(const reco::GsfElectron& ele,edm::Handle<reco::GenParticleCollection>& genParts);
};
  

HEEPV70Example::HEEPV70Example(const edm::ParameterSet& iPara)
{
  eleToken_=consumes<edm::View<reco::GsfElectron> >(iPara.getParameter<edm::InputTag>("eles"));
  vidToken_=consumes<edm::ValueMap<bool> >(iPara.getParameter<edm::InputTag>("vid"));
  trkIsolMapToken_=consumes<edm::ValueMap<float> >(iPara.getParameter<edm::InputTag>("trkIsolMap"));
  
}

void HEEPV70Example::beginJob()
{
 
}
void HEEPV70Example::endJob()
{
 
}

void HEEPV70Example::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<reco::GsfElectron> > eleHandle;
  edm::Handle<edm::ValueMap<bool> > vid;
  edm::Handle<edm::ValueMap<float> > trkIsolMap;

  iEvent.getByToken(eleToken_,eleHandle);
  iEvent.getByToken(vidToken_,vid);
  iEvent.getByToken(trkIsolMapToken_,trkIsolMap);

  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){  
    edm::Ptr<reco::GsfElectron> elePtr(eleHandle,eleNr);
   
    bool passHEEPV70=(*vid)[elePtr]; //passHEEPV70 = true, passes HEEP V7.0
    float trkIsol=(*trkIsolMap)[elePtr];
    std::cout <<"ele "<<elePtr->et()<<" "<<elePtr->eta()<<" "<<elePtr->phi()<<" trk isol: "<<trkIsol<<" pass HEEP V7.0"<<passHEEPV70<<std::endl;
  }
}

DEFINE_FWK_MODULE(HEEPV70Example);
