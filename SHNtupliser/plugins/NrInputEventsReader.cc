#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/MergeableCounter.h"

#include <iostream>

class NrInputEventsReader : public edm::EDAnalyzer {
  edm::EDGetTokenT<edm::MergeableCounter> nrEventsRunToken_;
  edm::EDGetTokenT<edm::MergeableCounter> nrEventsLumiToken_;
  unsigned int nrEventsRunSum_;
  unsigned int nrEventsLumiSum_;
  
public:
  
  NrInputEventsReader(const edm::ParameterSet& pset):nrEventsRunSum_(0),nrEventsLumiSum_(0){
    nrEventsRunToken_ = consumes<edm::MergeableCounter,edm::InRun>(pset.getParameter<edm::InputTag>("eventsRun"));
    nrEventsLumiToken_ = consumes<edm::MergeableCounter,edm::InLumi>(pset.getParameter<edm::InputTag>("eventsLumi"));
  }


  void beginRun(const edm::Run& run,const edm::EventSetup& setup ) override{}
   
  void endRun(const edm::Run& run,const edm::EventSetup& setup ) override{
    edm::Handle<edm::MergeableCounter> nrEventsHandle;
    run.getByToken(nrEventsRunToken_,nrEventsHandle);
    nrEventsRunSum_+=nrEventsHandle->value;
  }
 
  void endLuminosityBlock(const edm::LuminosityBlock& lumi,const edm::EventSetup& setup ) override{
    edm::Handle<edm::MergeableCounter> nrEventsHandle;
    lumi.getByToken(nrEventsLumiToken_,nrEventsHandle);
    nrEventsLumiSum_+=nrEventsHandle->value;
  }

  void analyze(const edm::Event& event,const edm::EventSetup& setup) override{
  }
  
  void endJob(){
    std::cout <<"total number of original events run "<<nrEventsRunSum_<<" lumi "<<nrEventsLumiSum_<<std::endl;
  }

 

};

DEFINE_FWK_MODULE(NrInputEventsReader);
