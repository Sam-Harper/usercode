#include "SHarper/HEEPAnalyzer/interface/HEEPIdValueMapProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

HEEPIdValueMapProducer::HEEPIdValueMapProducer(const edm::ParameterSet& iPara):
  cuts_(iPara)
{
  eleLabel_=iPara.getParameter<edm::InputTag>("eleLabel");
  applyRhoCorrToEleIsol_ = iPara.getParameter<bool>("applyRhoCorrToEleIsol");
  eleRhoCorrLabel_=iPara.getParameter<edm::InputTag>("eleRhoCorrLabel");
  verticesLabel_ = iPara.getParameter<edm::InputTag>("verticesLabel");
  writeIdAsInt_=iPara.getParameter<bool>("writeIdAsInt");
  if(writeIdAsInt_) produces < edm::ValueMap<int> >();
  else produces < edm::ValueMap<float> >(); //we had requests to write out as a float to be consistant with e/gamma 
}


//mostly stolen from or "inspired by" ElectronIDExternalProducer
void HEEPIdValueMapProducer::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  //load electrons
  edm::Handle<reco::GsfElectronCollection> eleHandle; 
  iEvent.getByLabel(eleLabel_,eleHandle);
  const reco::GsfElectronCollection& eles = *(eleHandle.product());

  
  edm::Handle<double> rhoHandle;
  iEvent.getByLabel(eleRhoCorrLabel_,rhoHandle);
  double rho = rhoHandle.isValid() ? *rhoHandle : 0;

  edm::Handle<reco::VertexCollection> verticesHandle;
  iEvent.getByLabel(verticesLabel_,verticesHandle);
  math::XYZPoint pvPos(0,0,0);
  if(!verticesHandle->empty()) pvPos = verticesHandle->front().position();


  //get the cut results for each electron
  std::vector<int> cutResults;
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    if(applyRhoCorrToEleIsol_) cutResults.push_back(cuts_.getCutCode(rho,pvPos,eles[eleNr]));
    else  cutResults.push_back(cuts_.getCutCode(eles[eleNr]));
  }
  
  if(writeIdAsInt_){
    std::auto_ptr<edm::ValueMap<int> > out(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler filler(*out);
    filler.insert(eleHandle,cutResults.begin(),cutResults.end());
    filler.fill();
    iEvent.put(out);
  }else{
    std::auto_ptr<edm::ValueMap<float> > out(new edm::ValueMap<float>());
    edm::ValueMap<float>::Filler filler(*out);
    filler.insert(eleHandle,cutResults.begin(),cutResults.end());
    filler.fill();
    iEvent.put(out);
  }
}



//define this as a plug-in
DEFINE_FWK_MODULE(HEEPIdValueMapProducer);
