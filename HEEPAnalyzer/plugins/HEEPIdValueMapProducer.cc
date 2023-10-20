

// This producer adds makes a value map with the heep selection results
//
//
// Original Author:  S. Harper 
//         Created: Tues Sep 2 2008

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPGsfEleExtraFiller.h"

class HEEPIdValueMapProducer : public edm::stream::EDProducer<> {

private:
  heep::EleSelector cuts_; //allows us to apply the heep selection 
  heep::GsfEleExtraFiller eleExtraFiller_;
  edm::EDGetTokenT<edm::View<reco::GsfElectron> >  eleToken_;
  bool writeIdAsInt_;

public:
  explicit HEEPIdValueMapProducer(const edm::ParameterSet& iPara);
  virtual ~HEEPIdValueMapProducer(){}
  
private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  
};


HEEPIdValueMapProducer::HEEPIdValueMapProducer(const edm::ParameterSet& iPara):
  cuts_(iPara),eleExtraFiller_(iPara,consumesCollector())
{
  eleToken_=consumes<edm::View<reco::GsfElectron> >(iPara.getParameter<edm::InputTag>("eleLabel"));
  writeIdAsInt_=iPara.getParameter<bool>("writeIdAsInt");
  if(writeIdAsInt_) produces < edm::ValueMap<int> >();
  else produces < edm::ValueMap<float> >(); //we had requests to write out as a float to be consistant with e/gamma 
}


//mostly stolen from or "inspired by" ElectronIDExternalProducer
void HEEPIdValueMapProducer::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  //load electrons
  edm::Handle<edm::View<reco::GsfElectron> > eleHandle; 
  iEvent.getByToken(eleToken_,eleHandle);

  eleExtraFiller_.getEvtContent(iEvent,iSetup);

  //get the cut results for each electron
  std::vector<int> cutResults;
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    edm::Ptr<reco::GsfElectron> ele = eleHandle->ptrAt(eleNr);
    cutResults.push_back(cuts_.getCutCode(*ele,eleExtraFiller_(ele)));
  }
  
  if(writeIdAsInt_){
    auto out = std::make_unique<edm::ValueMap<int>>();
    edm::ValueMap<int>::Filler filler(*out);
    filler.insert(eleHandle,cutResults.begin(),cutResults.end());
    filler.fill();
    iEvent.put(std::move(out));
  }else{
    auto out = std::make_unique<edm::ValueMap<float>>();
    edm::ValueMap<float>::Filler filler(*out);
    filler.insert(eleHandle,cutResults.begin(),cutResults.end());
    filler.fill();
    iEvent.put(std::move(out));
  }
}



//define this as a plug-in
DEFINE_FWK_MODULE(HEEPIdValueMapProducer);
