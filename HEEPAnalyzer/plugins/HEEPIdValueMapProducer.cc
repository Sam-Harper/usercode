#include "SHarper/HEEPAnalyzer/interface/HEEPIdValueMapProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"


HEEPIdValueMapProducer::HEEPIdValueMapProducer(const edm::ParameterSet& iPara):
  cuts_(iPara)
{
  eleToken_=consumes<edm::View<reco::GsfElectron> >(iPara.getParameter<edm::InputTag>("eleLabel"));
  applyRhoCorrToEleIsol_ = iPara.getParameter<bool>("applyRhoCorrToEleIsol");
  eleRhoCorrToken_=consumes<double>(iPara.getParameter<edm::InputTag>("eleRhoCorrLabel"));
  verticesToken_ = consumes<reco::VertexCollection>(iPara.getParameter<edm::InputTag>("verticesLabel"));
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
  
  edm::Handle<double> rhoHandle;
  iEvent.getByToken(eleRhoCorrToken_,rhoHandle);
  double rho = rhoHandle.isValid() ? *rhoHandle : 0;

  edm::Handle<reco::VertexCollection> verticesHandle;
  iEvent.getByToken(verticesToken_,verticesHandle);
  math::XYZPoint pvPos(0,0,0);
  if(!verticesHandle->empty()) pvPos = verticesHandle->front().position();


  //get the cut results for each electron
  std::vector<int> cutResults;
  for(auto& ele : *eleHandle){
    if(applyRhoCorrToEleIsol_) cutResults.push_back(cuts_.getCutCode(rho,pvPos,ele));
    else  cutResults.push_back(cuts_.getCutCode(ele));
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
