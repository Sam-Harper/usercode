
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"


class ECALPFClusterWithDummyCorrProducer : public edm::global::EDProducer<> {
public:
  ECALPFClusterWithDummyCorrProducer(const edm::ParameterSet& conf):
    srcToken_(consumes<reco::PFClusterCollection>(conf.getParameter<edm::InputTag>("src"))){
    produces<reco::PFClusterCollection>();
    produces<reco::PFCluster::EEtoPSAssociation>();
  }
  void produce(edm::StreamID id, edm::Event& event, edm::EventSetup const& setup) const{
    edm::Handle<reco::PFClusterCollection> src;
    event.getByToken(srcToken_,src);
    auto out = std::make_unique<reco::PFClusterCollection>();
    out->insert(out->end(),src->begin(),src->end());
    for(auto& clus : *out){
      clus.setCorrectedEnergy(clus.energy());
      clus.setCorrectedEnergyUncertainty(clus.energy()*0.01);
    }
    event.put(std::move(out));
    auto assoc = std::make_unique<reco::PFCluster::EEtoPSAssociation>();
    event.put(std::move(assoc));
    
  }
private:
  edm::EDGetTokenT<reco::PFClusterCollection> srcToken_;
};
DEFINE_FWK_MODULE(ECALPFClusterWithDummyCorrProducer);
