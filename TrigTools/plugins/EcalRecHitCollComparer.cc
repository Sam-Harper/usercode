#ifndef SHARPER_TRIGTOOLS_ECALRECHITCOLLCOMPARER
#define SHARPER_TRIGTOOLS_ECALRECHITCOLLCOMPARER

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include <vector>
#include <string>
#include <iostream>

namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}

class EcalRecHitCollComparer : public edm::stream::EDFilter<> {
private:

  edm::EDGetTokenT<EcalRecHitCollection> coll1Token_;
  edm::EDGetTokenT<EcalRecHitCollection> coll2Token_;
  edm::InputTag coll1Tag_;
  edm::InputTag coll2Tag_;
  

 
public:
  explicit EcalRecHitCollComparer(const edm::ParameterSet& iPara);
  ~EcalRecHitCollComparer(){}
  
  virtual bool filter(edm::Event& iEvent,const edm::EventSetup& iSetup)override;


};

EcalRecHitCollComparer::EcalRecHitCollComparer(const edm::ParameterSet& iPara)
{
  coll1Tag_=iPara.getParameter<edm::InputTag>("collection1");
  coll2Tag_=iPara.getParameter<edm::InputTag>("collection2");
  coll1Token_=consumes<EcalRecHitCollection>(coll1Tag_);
  coll2Token_=consumes<EcalRecHitCollection>(coll2Tag_);


}

bool EcalRecHitCollComparer::filter(edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<EcalRecHitCollection> coll1;
  edm::Handle<EcalRecHitCollection> coll2;
  
  iEvent.getByToken(coll1Token_,coll1);
  iEvent.getByToken(coll2Token_,coll2);
  
  bool different=false;
  for(auto& hitInColl1 : *coll1){
    auto hitInColl2 = coll2->find(hitInColl1.id());
    if(hitInColl2==coll2->end()){
      std::cout <<"hit "<<hitInColl1<<" was in collection "<<coll1Tag_.encode()<< "but not in collection "<<coll2Tag_.encode()<<std::endl;
      different=true;
    }
  }
  
  for(auto& hitInColl2 : *coll2){
    auto hitInColl1 = coll1->find(hitInColl2.id());
    if(hitInColl1==coll1->end()){
      std::cout <<"hit "<<hitInColl2<<" was in collection "<<coll2Tag_.encode()<< "but not in collection "<<coll1Tag_.encode()<<std::endl;
      different=true;
    }
  }
  if(!different) std::cout <<"perfect match "<<coll1Tag_.encode()<<" "<<coll1->size()<<" "<<coll2Tag_.encode()<<" "<<coll2->size()<<std::endl;
  return !different || true;

}
DEFINE_FWK_MODULE(EcalRecHitCollComparer);
#endif
