#include <iostream>
#include <iomanip>
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/L1Trigger/interface/EtSum.h" 
#include "DataFormats/L1Trigger/interface/Jet.h" 
#include "DataFormats/L1Trigger/interface/Tau.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "DataFormats/L1TGlobal/interface/GlobalExtBlk.h"

//remakes the L1 produces so they have the current process
//horrible hack to fix a trigger AOD summary issue


class L1Stage2FakeProducer : public edm::stream::EDProducer<> {
public:
  explicit L1Stage2FakeProducer(const edm::ParameterSet&);
  ~L1Stage2FakeProducer(){}
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:
  virtual void produce(edm::Event&, const edm::EventSetup&) override;
   
private:
  template<typename T>
  void getToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& pset,const std::string& name){
    token = consumes<T>(pset.getParameter<edm::InputTag>(name));
  }

  edm::EDGetTokenT<l1t::EGammaBxCollection> egToken_;
  edm::EDGetTokenT<l1t::EtSumBxCollection> etSumToken_;
  edm::EDGetTokenT<l1t::JetBxCollection> jetToken_;
  edm::EDGetTokenT<l1t::MuonBxCollection> muonToken_;
  edm::EDGetTokenT<l1t::TauBxCollection> tauToken_;
  edm::EDGetTokenT<GlobalAlgBlkBxCollection> algBlkToken_;
  edm::EDGetTokenT<GlobalExtBlkBxCollection> extBlkToken_;
  
};

namespace {
  
}

L1Stage2FakeProducer::L1Stage2FakeProducer(const edm::ParameterSet& pset)
{
  getToken(egToken_,pset,"egamma");
  getToken(muonToken_,pset,"muon");
  getToken(etSumToken_,pset,"etSum");
  getToken(jetToken_,pset,"jet");
  getToken(tauToken_,pset,"tau");
  getToken(algBlkToken_,pset,"algBlk");
  getToken(extBlkToken_,pset,"extBlk");
 
  produces<l1t::MuonBxCollection>("Muon");
  produces<l1t::EGammaBxCollection>("EGamma");
  produces<l1t::EtSumBxCollection>("EtSum");
  produces<l1t::JetBxCollection>("Jet");
  produces<l1t::TauBxCollection>("Tau");
  produces<GlobalAlgBlkBxCollection>();
  produces<GlobalExtBlkBxCollection>();
}
void L1Stage2FakeProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("egamma", edm::InputTag("hltGtStage2Digis","EGamma"));
  desc.add<edm::InputTag>("etSum", edm::InputTag("hltGtStage2Digis","EtSum"));
  desc.add<edm::InputTag>("muon", edm::InputTag("hltGtStage2Digis","Muon"));
  desc.add<edm::InputTag>("jet", edm::InputTag("hltGtStage2Digis","Jet"));
  desc.add<edm::InputTag>("tau", edm::InputTag("hltGtStage2Digis","Tau"));
  desc.add<edm::InputTag>("algBlk", edm::InputTag("hltGtStage2Digis"));
  desc.add<edm::InputTag>("extBlk", edm::InputTag("hltGtStage2Digis"));
  
  descriptions.add("l1Stage2FakeProducer", desc);
}


namespace{
  template<typename T>
    void process(edm::Event& event,const edm::EDGetTokenT<T>& token,std::string name)
  {
    edm::Handle<T> handle;
    event.getByToken(token,handle);
    auto outColl = std::make_unique<T>(*handle);
    event.put(std::move(outColl),std::move(name));
  }
}
void L1Stage2FakeProducer::produce(edm::Event& event,const edm::EventSetup& setup)
{
  process(event,muonToken_,"Muon");
  process(event,egToken_,"EGamma");
  process(event,etSumToken_,"EtSum");
  process(event,jetToken_,"Jet"); 
  process(event,tauToken_,"Tau"); 
  process(event,algBlkToken_,""); 
  process(event,extBlkToken_,""); 
}

DEFINE_FWK_MODULE(L1Stage2FakeProducer);
