
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "SHarper/TrigTools/interface/TrigToolsStructs.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TTree.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class EGammaDebugger : public edm::EDAnalyzer { 

public:
  explicit EGammaDebugger(const edm::ParameterSet& iPara);
  virtual ~EGammaDebugger();
  
  EGammaDebugger(const EGammaDebugger& rhs)=delete;
  EGammaDebugger& operator=(const EGammaDebugger& rhs)=delete;

 private:
  template<typename T>
  void getToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& paraName){
    token = consumes<T>(iPara.getParameter<edm::InputTag>(paraName));
  }
  
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup){}
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  
private:
  edm::EDGetTokenT<std::vector<pat::Electron> > elesToken_;
  edm::EDGetTokenT<std::vector<pat::Photon> > phosToken_;
  edm::EDGetTokenT<std::vector<pat::Photon> > ootPhosToken_;

};


namespace{
  template<typename T> edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token){
    edm::Handle<T> handle;
    iEvent.getByToken(token,handle);
    return handle;
  }

  template<typename T> void print(const edm::Event& evt,const std::string& tag,const T& obj){
    std::cout <<tag<<" event "<<evt.id().run()<<" "<<evt.id().event()<<" "<<evt.bunchCrossing()<<" ";
    std::cout <<" pt/eta/phi "<<obj.pt()<<" "<<obj.eta()<<" "<<obj.phi()<<" ecal "<<obj.ecalPFClusterIso()<<" hcal "<<obj.hcalPFClusterIso()<<std::endl;
  }
}

EGammaDebugger::EGammaDebugger(const edm::ParameterSet& iPara)
{
  getToken(elesToken_,iPara,"patElectrons");
  getToken(phosToken_,iPara,"patPhotons");
  getToken(ootPhosToken_,iPara,"patOOTPhotons");
  
}

EGammaDebugger::~EGammaDebugger(){}

void EGammaDebugger::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  auto elesHandle = getHandle(iEvent,elesToken_);
  auto phosHandle = getHandle(iEvent,phosToken_);
  auto ootPhosHandle = getHandle(iEvent,ootPhosToken_);
  
  for(const auto & obj : *elesHandle){
    print(iEvent,"egammaDebug: ele",obj);
  }
  for(const auto & obj : *phosHandle){
    print(iEvent,"egammaDebug: pho",obj);
  }
  for(const auto & obj : *ootPhosHandle){
    print(iEvent,"egammaDebug: ootpho",obj);
  }
  
  
}

DEFINE_FWK_MODULE(EGammaDebugger);
