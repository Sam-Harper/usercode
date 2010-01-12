#include "SHarper/HEEPAnalyzer/interface/HEEPIdValueMapProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

HEEPIdValueMapProducer::HEEPIdValueMapProducer(const edm::ParameterSet& iPara):
  cuts_(iPara)
{
  eleLabel_=iPara.getParameter<edm::InputTag>("eleLabel");

  produces < edm::ValueMap<int> >();
}


//mostly stolen from or "inspired by" ElectronIDExternalProducer
void HEEPIdValueMapProducer::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  //load electrons
  edm::Handle<reco::GsfElectronCollection> eleHandle; 
  iEvent.getByLabel(eleLabel_,eleHandle);
  const reco::GsfElectronCollection& eles = *(eleHandle.product());

  //prepare output collection
  std::auto_ptr<edm::ValueMap<int> > out(new edm::ValueMap<int>());

  //get the cut results for each electron
  std::vector<int> cutResults;
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    cutResults.push_back(cuts_.getCutCode(eles[eleNr]));
  }
  
  edm::ValueMap<int>::Filler filler(*out);
  filler.insert(eleHandle,cutResults.begin(),cutResults.end());
  filler.fill();

  //store the output
  iEvent.put(out);
}



//define this as a plug-in
DEFINE_ANOTHER_FWK_MODULE(HEEPIdValueMapProducer);
