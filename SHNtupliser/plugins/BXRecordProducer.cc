#ifndef SHarper_BXRecordProducer_h
#define SHarper_BXRecordProducer_h

#include <map>

#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"



class BXRecordProducer : public edm::stream::EDProducer<>
{
  int bunchSpacing_;

public:
  
  explicit BXRecordProducer( const edm::ParameterSet & config ):
    bunchSpacing_(config.getParameter<int>("bunchSpacing")){
    produces<int>("bunchSpacing");
  }
  
private:
  
  void produce( edm::Event & event, const edm::EventSetup & ) override{
     std::auto_ptr<int> bunchSpacingP(new int(bunchSpacing_));
     event.put(bunchSpacingP,"bunchSpacing");
  }

};
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(BXRecordProducer);

#endif
