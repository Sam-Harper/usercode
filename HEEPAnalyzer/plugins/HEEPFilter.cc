

#include "FWCore/Framework/interface/EDFilter.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEventHelper.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class HEEPFilter : public edm::EDFilter {

private:
  heep::EventHelper evtHelper_; //this is our magic class where all the nastyness is contained
  heep::Event heepEvt_;

 

  //disabling copy and assignment 
  //this class is in theory copyable but it seems odd to allow it
private:
  HEEPFilter(const HEEPFilter& rhs){}
  HEEPFilter& operator=(const HEEPFilter& rhs){return *this;}

public:
  explicit HEEPFilter(const edm::ParameterSet& iPara):evtHelper_(),heepEvt_(){
    evtHelper_.setup(iPara);
  }
  virtual ~HEEPFilter(){}
  
private:
  virtual bool filter(edm::Event& iEvent, const edm::EventSetup& iSetup);
  
};

bool HEEPFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  evtHelper_.makeHeepEvent(iEvent,iSetup,heepEvt_);
  
  const std::vector<heep::Ele>& eles = heepEvt_.heepEles();
  for(size_t eleNr1=0;eleNr1<eles.size();eleNr1++){
    for(size_t eleNr2=eleNr1+1;eleNr2<eles.size();eleNr2++){
      if((eles[eleNr1].cutCode()&-2)==0x0 && (eles[eleNr2].cutCode()&-2)==0x0) return true;
    }
  }
  return false;
}

DEFINE_FWK_MODULE(HEEPFilter);
