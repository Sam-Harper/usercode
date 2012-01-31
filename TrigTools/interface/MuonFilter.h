#ifndef SHARPER_TRIGTOOLS_MUONFILTER
#define SHARPER_TRIGTOOLS_MUONFILTER


#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"
#include <vector>
#include <string>
#include <iostream>


namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}

class MuonFilter : public edm::EDFilter {
private:
  int nrPass_;

  float ptCut1_,ptCut2_;
  int nrToPassCut1_,nrToPassCut2_;
  edm::InputTag muonTag_;

public:
  explicit MuonFilter(const edm::ParameterSet& iPara);
  ~MuonFilter(){}
  
  virtual bool filter(edm::Event& iEvent,const edm::EventSetup& iSetup);
  virtual void endJob(){std::cout <<" nr pass "<<nrPass_<<std::endl;}
  

};

#endif
