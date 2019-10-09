
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"



class EmptyESAssocProducer : public edm::global::EDProducer<> {
public:
  EmptyESAssocProducer(const edm::ParameterSet& ){produces<reco::PFCluster::EEtoPSAssociation>();}
  void produce(edm::StreamID id, edm::Event& event, edm::EventSetup const& setup) const{
    auto assoc = std::make_unique<reco::PFCluster::EEtoPSAssociation>();
    event.put(std::move(assoc));
    
  }
};
DEFINE_FWK_MODULE(EmptyESAssocProducer);
