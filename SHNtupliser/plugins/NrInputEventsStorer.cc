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


class NrInputEventsStorer : public edm::stream::EDProducer<edm::RunCache<Count>,edm::EndRunProducer,edm::LuminosityBlockSummaryCache<Count>,edm::EndLuminosityBlockProducer> {
  
  int lumiCount_;
public:
  
  NrInputEventsStorer(const edm::ParameterSet& pset):lumiCount_(0){
    produces<edm::MergeableCounter,edm::InRun>("nrEventsRun");
    produces<edm::MergeableCounter,edm::InLumi>("nrEventsLumi");
  }

  static std::shared_ptr<Count> globalBeginRun(const edm::Run& run,const edm::EventSetup& setup, const GlobalCache* ){
    return std::shared_ptr<Count>(new Count);
  }

  static void globalEndRun(const edm::Run& run,const edm::EventSetup& setup,const RunContext*){}
  
  static std::shared_ptr<Count> globalBeginLuminosityBlockSummary(const edm::LuminosityBlock& lumi,const edm::EventSetup& setup, const GlobalCache* ){
    return std::shared_ptr<Count>(new Count);
  }
 
  void produce(edm::Event& event,const edm::EventSetup& setup) override{
    ++(runCache()->value);
    lumiCount_++;
  }
  

  static void globalEndRunProduce(edm::Run& run,const edm::EventSetup& setup,const RunContext* context ){
    int value = context->run()->value.load();
    auto nrEventsForRun = std::make_unique<edm::MergeableCounter>();
    nrEventsForRun->value=value;
    run.put(std::move(nrEventsForRun),"nrEventsRun");
  }

  //this resets the lumi count to zero for this module
  void beginLuminosityBlock(const edm::LuminosityBlock& lumi,
			    const edm::EventSetup& setup){
    lumiCount_=0;
  }

  //my understanding is that here all modules which have seen this lumi call
  //this function which then summs all the individual module counts into the 
  //master count for this lumisection
  //ah and the global one is static so it has to happen here!
  void endLuminosityBlockSummary(const edm::LuminosityBlock& lumi,
				 const edm::EventSetup& setup,
				 Count* count)const override{
    count->value+=lumiCount_;
  }
  
  
  static void globalEndLuminosityBlockSummary(const edm::LuminosityBlock& lumi,
					      const edm::EventSetup& setup,
					      const LuminosityBlockContext* context,
					      Count* count){
  }
    
  //now I think this gets called once and the Count object is now completely filled
  //so we then write the object
  static void globalEndLuminosityBlockProduce(edm::LuminosityBlock& lumi,
					      const edm::EventSetup& setup,
					      const LuminosityBlockContext* context,
					      const Count* count){
    int value = count->value.load();
    auto nrEventsForLumi = std::make_unique<edm::MergeableCounter>();
    nrEventsForLumi->value=value;
    lumi.put(std::move(nrEventsForLumi),"nrEventsLumi");
  }

};

DEFINE_FWK_MODULE(NrInputEventsStorer);
