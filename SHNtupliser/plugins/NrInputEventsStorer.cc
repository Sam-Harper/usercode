#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "FWCore/ServiceRegistry/interface/GlobalContext.h"
#include "DataFormats/Common/interface/MergeableCounter.h"
#include <iostream>

struct Count {
  Count():value(0){}
  mutable std::atomic<int> value;
};


class NrInputEventsStorer : public edm::stream::EDProducer<edm::RunCache<Count>,edm::EndRunProducer> {
  
public:
  
  NrInputEventsStorer(const edm::ParameterSet& pset){
    produces<edm::MergeableCounter,edm::Transition::EndRun>("nrEventsRun");
    // produces<edm::MergeableCounter,edm::InLumi>("nrEventsLumi");
  }

  static std::shared_ptr<Count> globalBeginRun(const edm::Run& run,const edm::EventSetup& setup, const GlobalCache* ){
    return std::shared_ptr<Count>(new Count);
  }

  static void globalEndRun(const edm::Run& run,const edm::EventSetup& setup,const RunContext*){}
  
 
  void produce(edm::Event& event,const edm::EventSetup& setup) override{
    ++(runCache()->value);
  }
  

  static void globalEndRunProduce(edm::Run& run,const edm::EventSetup& setup,const RunContext* context ){
    int value = context->run()->value.load();
    auto nrEventsForRun = std::make_unique<edm::MergeableCounter>();
    nrEventsForRun->value=value;
    run.put(std::move(nrEventsForRun),"nrEventsRun");
  }


};

DEFINE_FWK_MODULE(NrInputEventsStorer);
