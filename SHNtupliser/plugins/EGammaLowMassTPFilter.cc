
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaTowerIsolation.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "TTree.h"


class EGammaLowMassTPFilter : public edm::one::EDFilter<> {

private:
  edm::EDGetTokenT<edm::View<pat::Electron> > eleTag_;
  edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoTag_;
  std::string tagId_;
  float minMass_;
  float maxMass_;
  
public:
  explicit EGammaLowMassTPFilter(const edm::ParameterSet& para);
  ~EGammaLowMassTPFilter(){}
  
  virtual bool filter(edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();
};

EGammaLowMassTPFilter::EGammaLowMassTPFilter(const edm::ParameterSet& para)
{
  eleTag_=consumes<edm::View<pat::Electron> >(para.getParameter<edm::InputTag>("eleTag"));
  tagId_ = para.getParameter<std::string>("tagId");
  minMass_ = para.getParameter<double>("minMass");
  maxMass_ = para.getParameter<double>("maxMass");
  
}

bool EGammaLowMassTPFilter::filter(edm::Event& event,const edm::EventSetup& setup)
{   
  
  edm::Handle<edm::View<pat::Electron> > eleHandle;
  event.getByToken(eleTag_,eleHandle);
  
  const auto& eles = *eleHandle;

  for(size_t ele1Nr=0;ele1Nr<eles.size();ele1Nr++){
    for(size_t ele2Nr=ele1Nr+1;ele2Nr<eles.size();ele2Nr++){
      const auto& ele1 = eles[ele1Nr];
      const auto& ele2 = eles[ele2Nr];
      
      if(ele1.electronID(tagId_) || ele2.electronID(tagId_)){
	float mass = (ele1.p4() + ele2.p4()).mag();
	if(mass>minMass_ && mass<maxMass_) return true;
      }
    }
  }

  return false;
  
}
void EGammaLowMassTPFilter::endJob()
{ 
  
}  




DEFINE_FWK_MODULE(EGammaLowMassTPFilter);
