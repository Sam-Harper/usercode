
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"

class SinglePhoSkimmer : public edm::EDFilter {

public:
  

private:
  edm::InputTag trigEventTag_;
  std::vector<std::string> singlePhoLastFilterNames_;
  std::vector<int> preScales_;
  int nrEtBins_;
  float minEt_;
  float maxEt_;

  int nrPass_;
  int nrTot_;

public:
  explicit SinglePhoSkimmer(const edm::ParameterSet&);
  ~SinglePhoSkimmer(){}
  
  virtual void beginJob();
  virtual bool filter(edm::Event& ,const edm::EventSetup& );
  virtual void endJob(){}
  
  int getPreScale(float et);
  size_t getBinNr(int nrBins,double min,double max,double x);

};

SinglePhoSkimmer::SinglePhoSkimmer(const edm::ParameterSet& iConfig)
{
  trigEventTag_ = iConfig.getParameter<edm::InputTag>("trigEventTag");
  singlePhoLastFilterNames_ = iConfig.getParameter<std::vector<std::string> >("singlePhoLastFilterNames");
  preScales_ = iConfig.getParameter<std::vector<int> >("preScales");
  nrEtBins_ = iConfig.getParameter<int>("nrEtBins");
  minEt_ = iConfig.getParameter<double>("minEt");
  maxEt_ = iConfig.getParameter<double>("maxEt");
  if(preScales_.size()!=static_cast<size_t>(nrEtBins_+2)) std::cout <<"Warnining, input vector size "<<preScales_.size()<<" and nr bins "<<nrEtBins_<<" missmatch "<<std::endl; 

  produces<int>();
}

void SinglePhoSkimmer::beginJob()
{ 

}

bool SinglePhoSkimmer::filter(edm::Event& iEvent,const edm::EventSetup& iSetup )
{
  edm::Handle<trigger::TriggerEvent> trigEvtHandle;
  iEvent.getByLabel(trigEventTag_,trigEvtHandle);
  
  //now need to find max et object passing a single photon trigger
  //brute force (assumes no sorting or clever ordering of paths) for the win!
  float maxEt=-1;
  for(size_t filterNr=0;filterNr<singlePhoLastFilterNames_.size();filterNr++){
    
    std::vector<math::XYZTLorentzVector> p4s;
    trigtools::getP4sOfObsPassingFilter(p4s,(*trigEvtHandle),singlePhoLastFilterNames_[filterNr],trigEventTag_.process());
    
    for(size_t objNr=0;objNr<p4s.size();objNr++){
      if(p4s[objNr].Et()>maxEt) maxEt = p4s[objNr].Et();
    }
  }

  int preScale = getPreScale(maxEt);

  std::auto_ptr<int> preScalePointer(new int(preScale));
  iEvent.put(preScalePointer);

  // if(maxEt!=-1 && iEvent.id().event()%preScale==0) std::cout <<"selectedEvent: "<<iEvent.id().run()<<" "<<iEvent.luminosityBlock()<<" "<<iEvent.id().event()<<std::endl;
  return maxEt!=-1 && iEvent.id().event()%preScale==0;

}

int SinglePhoSkimmer::getPreScale(float et)
{
  size_t binNr=getBinNr(nrEtBins_,minEt_,maxEt_,et);
  if(binNr<preScales_.size()) return preScales_[binNr];
  else return 1;
}

size_t SinglePhoSkimmer::getBinNr(int nrBins,double min,double max,double x)
{
  //uses root logic
  const float step = (max-min)/nrBins;

  //first over, underflow
  if(x<min) return 0; 
  if(x>=max) return nrBins+1;
  
  for(int binNr=1;binNr<=nrBins;binNr++){
    float lowEdge = step*(binNr-1)+min;
    float highEdge = step*(binNr)+min;
    if(x>=lowEdge && x<highEdge) return binNr;
  }
  return nrBins+2;
}

DEFINE_FWK_MODULE(SinglePhoSkimmer);
