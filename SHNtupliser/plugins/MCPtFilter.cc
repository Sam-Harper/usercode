
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include <limits>

//very basic filter intended for particle gun sample to filter out electrons of a given pt
//as its just works for particle guns, doesnt do a lot of additional required checks

class MCPtFilter : public edm::stream::EDFilter<> {

private:
  edm::EDGetTokenT<reco::GenParticleCollection> genPartsToken_;
  int pid_;
  double minPt_;
  double maxPt_;
  
public:
  explicit MCPtFilter(const edm::ParameterSet& para);
  ~MCPtFilter(){}
  
  virtual bool filter(edm::Event& event,const edm::EventSetup& setup);
  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
};

MCPtFilter::MCPtFilter(const edm::ParameterSet& para)
{
  genPartsToken_=consumes<reco::GenParticleCollection>(para.getParameter<edm::InputTag>("genParts"));
  pid_ = para.getParameter<int>("pid");
  minPt_ = para.getParameter<double>("minPt");
  maxPt_ = para.getParameter<double>("maxPt");
}

void MCPtFilter::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("genParts",edm::InputTag("genParticles"));
  desc.add<int>("pid", 11);
  desc.add<double>("minPt", 0.0);
  desc.add<double>("maxPt", std::numeric_limits<double>::max());
  descriptions.add("produceMuons", desc);
}

bool MCPtFilter::filter(edm::Event& event,const edm::EventSetup& setup)
{   
  

  edm::Handle<reco::GenParticleCollection> genPartsHandle;
  event.getByToken(genPartsToken_,genPartsHandle);
  for(const auto& part : *genPartsHandle){
    if(std::abs(part.pdgId())==pid_ && part.pt()>=minPt_ && part.pt()<maxPt_) return true;
  }
  return false;
}  


DEFINE_FWK_MODULE(MCPtFilter);
