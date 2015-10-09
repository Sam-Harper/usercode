
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaTowerIsolation.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "TTree.h"


class EleFilter : public edm::EDProducer {

private:
 
  edm::InputTag eleTag_;

public:
  explicit EleFilter(const edm::ParameterSet& para);
  ~EleFilter(){}
  
  virtual void produce(edm::Event& event,const edm::EventSetup& setup) override;
};

EleFilter::EleFilter(const edm::ParameterSet& para)

{
  eleTag_=para.getParameter<edm::InputTag>("eleTag");
  produces<reco::GsfElectronCollection>();

}

void EleFilter::produce(edm::Event& event,const edm::EventSetup& setup)
{   
  edm::Handle<reco::GsfElectronCollection> eles;
  event.getByLabel(eleTag_,eles);
  
  std::auto_ptr<reco::GsfElectronCollection> newEles(new reco::GsfElectronCollection);
  for(const auto& ele : *eles){
    if(ele.et()>100) newEles->push_back(ele);
  }
  event.put(newEles);
}




DEFINE_FWK_MODULE(EleFilter);
