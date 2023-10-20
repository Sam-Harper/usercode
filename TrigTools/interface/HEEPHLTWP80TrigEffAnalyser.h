#ifndef SHARPER_TRIGTOOLS_HEEPHLTWP80TrigEffAnalyser
#define SHARPER_TRIGTOOLS_HEEPHLTWP80TrigEffAnalyser


//this is a simple class which simulates another trigger based on the already calculated trigger object P4s and filters on that

#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
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


//this class is a simple example on how to access the p4 of objects passing a given trigger
//it also shows you how to put in a path name and get the filters availible in trigger event that have the p4s stored

//things to note:
//the only requirements for an object to pass a filter is
//1) the filter to be run
//2) the object to pass it
//Just because objects passed it does not mean the path you are interested in passes
//There are two cases where this is not true. In paths which require more than one object to pass
//it will save every object that meets requirements. If the number of objects required by the path is more
//than passing the filter, the path will not accept but the filter is saved regardless. Then all that is needed
//is that another path accepts so the event is saved. 
//The other case is that multiple paths can (and are encouraged to) share the same filter. This means that the other 
//path may have passed not thiat path. An example is the PFMHT150 has the hltMET80 used by CentralJet80_MET80 which
// means that you could see an object passing hltMET80 but not pass CentralJet80_MET80. This is rare but an effect 
//you should be aware of

class HEEPHLTWP80TrigEffAnalyser: public edm::one::EDAnalyzer<> {


private:
  edm::InputTag trigEventTag_;
  std::string tagFilterName_;
  std::string probeFilterName_;
  std::string wp80FilterName_;

 
  HLTConfigProvider hltConfig_; //to translate path names to filter names
 

public:
  explicit HEEPHLTWP80TrigEffAnalyser(const edm::ParameterSet& iPara);
  ~HEEPHLTWP80TrigEffAnalyser(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  
  

};

#endif
