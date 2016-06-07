
// This producer adds the HEEPSelector status word as userdate to the PAT electron collection
//
//
// Original Author:  S. Harper / M.Mozer
//         Created: Tues Sep 2 2008

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPGsfEleExtraFiller.h"

class HEEPAttStatusToPAT : public edm::EDProducer {

private:
  heep::EleSelector cuts_; //allows us to apply the heep selection
  heep::GsfEleExtraFiller eleExtraFiller_;
  edm::EDGetTokenT<edm::View<pat::Electron>> eleToken_;
  

public:
  explicit HEEPAttStatusToPAT(const edm::ParameterSet& iPara);
  virtual ~HEEPAttStatusToPAT(){}
  
private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  
};

HEEPAttStatusToPAT::HEEPAttStatusToPAT(const edm::ParameterSet& iPara):
  cuts_(iPara),eleExtraFiller_(iPara,consumesCollector())
{
  eleToken_=consumes<edm::View<pat::Electron> >(iPara.getParameter<edm::InputTag>("eleLabel")); 
  produces < pat::ElectronCollection >();
}


void HEEPAttStatusToPAT::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  //load electrons
  edm::Handle<edm::View<pat::Electron> > eleHandle;
  iEvent.getByToken(eleToken_,eleHandle);
  const edm::View<pat::Electron>& eles = *(eleHandle.product());
  eleExtraFiller_.getEvtContent(iEvent,iSetup);
  

  //prepare output collection
  std::auto_ptr<pat::ElectronCollection> outEle(new pat::ElectronCollection());

  //clone electrons, reset energy to the ecal energy and add info
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    pat::Electron * newEle = eles[eleNr].clone();
    newEle->setP4(newEle->p4()/newEle->energy()*newEle->ecalEnergy());
    newEle->addUserInt("HEEPId",cuts_.getCutCode(eles[eleNr],eleExtraFiller_(eles.ptrAt(eleNr))));
    
    outEle->push_back(*(newEle));
    delete newEle;
  }

  //store the output
  iEvent.put(outEle);
}



//define this as a plug-in
DEFINE_FWK_MODULE(HEEPAttStatusToPAT);
