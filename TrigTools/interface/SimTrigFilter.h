#ifndef SHARPER_TRIGTOOLS_SIMTRIGFILTER
#define SHARPER_TRIGTOOLS_SIMTRIGFILTER


//this is a simple class which simulates another trigger based on the already calculated trigger object P4s and filters on that

#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include <vector>
#include <string>

namespace trigger{
  class TriggerEvent;
}

namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}

class SimTrigFilter : public edm::EDFilter {
private:
  class TrigCut {
    std::string filterName_;
    std::string processName_;
    float ptCut_;
    float nrObjs_;
  public:
    TrigCut(const std::string& cuts,const std::string& processName="HLT"); //format filterName:Pt:NrObjects required
    ~TrigCut(){}
    
    bool passCut(const trigger::TriggerEvent* trigEvent)const;

  };


private:
  edm::InputTag trigEventTag_;
  //  edm::InputTag trigResultsTag_;


  std::vector<TrigCut> trigCuts_; //format filterName:Pt:NrObjects required
public:
  explicit SimTrigFilter(const edm::ParameterSet& iPara);
  ~SimTrigFilter(){}
  
  virtual bool filter(edm::Event& iEvent,const edm::EventSetup& iSetup);
  

};

#endif
