
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
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

template <typename T>
class EGammaAODDebugger : public edm::one::EDAnalyzer<> { 

public:
  explicit EGammaAODDebugger(const edm::ParameterSet& iPara);
  virtual ~EGammaAODDebugger();
  
  EGammaAODDebugger(const EGammaAODDebugger& rhs)=delete;
  EGammaAODDebugger& operator=(const EGammaAODDebugger& rhs)=delete;

 private:
  template<typename T1>
  void getToken(edm::EDGetTokenT<T1>& token,const edm::ParameterSet& iPara,const std::string& paraName){
    token = consumes<T1>(iPara.getParameter<edm::InputTag>(paraName));
  }
  
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup){}
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  
private:
  static std::string objStr(){return "obj";}
private:
  edm::EDGetTokenT<std::vector<T> > objsToken_;
  std::vector<edm::EDGetTokenT<edm::ValueMap<float> > > energyCalibTokens_;
  std::vector<edm::EDGetTokenT<edm::ValueMap<float> > > vidPassTokens_;
  std::vector<edm::EDGetTokenT<edm::ValueMap<unsigned int> > > vidBitMaskTokens_;
  std::vector<std::string> energyCalibNames_;
  std::string energyCalibProducer_;
  std::string vidProducer_;
  std::vector<std::string> vidNames_;

};


namespace{
  template<typename T> edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token){
    edm::Handle<T> handle;
    iEvent.getByToken(token,handle);
    return handle;
  }
}
template<typename T>
EGammaAODDebugger<T>::EGammaAODDebugger(const edm::ParameterSet& iPara)
{
  getToken(objsToken_,iPara,"src");
  energyCalibNames_ = iPara.getParameter<std::vector<std::string> >("energyCalibNames");
  energyCalibProducer_ = iPara.getParameter<std::string>("energyCalibProducer");
  vidNames_ = iPara.getParameter<std::vector<std::string> >("vidNames");
  vidProducer_ = iPara.getParameter<std::string>("vidProducer");

  for(const auto& name : energyCalibNames_){
    energyCalibTokens_.emplace_back(consumes<edm::ValueMap<float> >(edm::InputTag(energyCalibProducer_,name)));
  }
 for(const auto& name : vidNames_){
    vidPassTokens_.emplace_back(consumes<edm::ValueMap<float> >(edm::InputTag(vidProducer_,name)));
    vidBitMaskTokens_.emplace_back(consumes<edm::ValueMap<unsigned int> >(edm::InputTag(vidProducer_,name+"Bitmap")));
  }

}

template<typename T>
EGammaAODDebugger<T>::~EGammaAODDebugger(){}

template<>
std::string EGammaAODDebugger<reco::GsfElectron>::objStr()
{
  return "ele";
}
template<>
std::string EGammaAODDebugger<reco::Photon>::objStr()
{
  return "pho";
}

template<typename T>
void EGammaAODDebugger<T>::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  auto objsHandle = getHandle(iEvent,objsToken_);

  std::cout <<"event "<<iEvent.id().run()<<" "<<iEvent.luminosityBlock()<<iEvent.id().event()<<" "<<iEvent.bunchCrossing()<<(" nr "+objStr())<<" "<<objsHandle->size()<<std::endl;
  for(size_t objNr=0;objNr<objsHandle->size();objNr++){
    edm::Ref<std::vector<T> > objRef(objsHandle,objNr);
    
    std::cout <<" E/pt/eta/phi "<<objRef->energy()<<" "<<objRef->et()<<" "<<objRef->eta()<<" "<<objRef->phi()<<std::endl;
    std::cout <<"   VIDs: "<<std::endl;
    for(size_t vidNr=0;vidNr<vidNames_.size();vidNr++){
      const auto& vidPass = *getHandle(iEvent,vidPassTokens_[vidNr]).product();
      const auto& vidBits = *getHandle(iEvent,vidBitMaskTokens_[vidNr]).product();
      std::cout <<"      "<<vidNames_[vidNr]<<" "<<vidPass[objRef]<<" "<<vidBits[objRef]<<std::endl;
    }
    std::cout <<"   energy calib: "<<std::endl;
    for(size_t calibNr=0;calibNr<energyCalibNames_.size();calibNr++){
      const auto& energyCalib = *getHandle(iEvent,energyCalibTokens_[calibNr]).product();

      std::cout <<"      "<<energyCalibNames_[calibNr]<<" "<<energyCalib[objRef]<<std::endl;
    }
  }
   
}

using EGammaAODEleDebugger=EGammaAODDebugger<reco::GsfElectron>;
using EGammaAODPhoDebugger=EGammaAODDebugger<reco::Photon>;
DEFINE_FWK_MODULE(EGammaAODEleDebugger);
DEFINE_FWK_MODULE(EGammaAODPhoDebugger);
