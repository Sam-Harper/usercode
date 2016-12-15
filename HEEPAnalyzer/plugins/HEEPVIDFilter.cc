#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "FWCore/Framework/interface/MakerMacros.h"


#include "TH1D.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

template <typename ObjType,typename CollType=std::vector<ObjType> >
class HEEPVIDFilter : public edm::stream::EDFilter<> {

private:
 
  const edm::EDGetTokenT<edm::View<ObjType> > objToken_;
  const edm::EDGetTokenT<edm::ValueMap<bool> > vidToken_; //VID is versioned ID, is the standard E/gamma ID producer which we have configured for HEEP
  const int minNrRequired_;
public:
  explicit HEEPVIDFilter(const edm::ParameterSet& iPara);
  virtual ~HEEPVIDFilter(){}
  
private:
  bool filter(edm::Event& iEvent,const edm::EventSetup& iSetup) override;
};
  
template <typename ObjType,typename CollType>
HEEPVIDFilter<ObjType,CollType>::HEEPVIDFilter(const edm::ParameterSet& iPara):
  objToken_(consumes<edm::View<ObjType> >(iPara.getParameter<edm::InputTag>("src"))),
  vidToken_(consumes<edm::ValueMap<bool> >(iPara.getParameter<edm::InputTag>("vid"))),
  minNrRequired_(iPara.getParameter<int>("minNrRequired"))
{
  produces<CollType>();
}


template <typename ObjType,typename CollType>
bool HEEPVIDFilter<ObjType,CollType>::filter(edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  
  edm::Handle<edm::View<ObjType> > objHandle;
  edm::Handle<edm::ValueMap<bool> > vid;
  

  iEvent.getByToken(objToken_,objHandle);
  iEvent.getByToken(vidToken_,vid);

  auto outColl = std::make_unique<CollType>();
  
  for(size_t objNr=0;objNr<objHandle->size();objNr++){  
    edm::Ptr<ObjType> objPtr(objHandle,objNr);
   
    bool passVID=(*vid)[objPtr]; 
    if(passVID) outColl->push_back(*objPtr);
  }
  int nrPass = outColl->size();
  iEvent.put(std::move(outColl));
  return nrPass>=minNrRequired_;
}

using HEEPGsfEleVIDFilter=HEEPVIDFilter<reco::GsfElectron>;
using HEEPPatEleVIDFilter=HEEPVIDFilter<pat::Electron>;


DEFINE_FWK_MODULE(HEEPGsfEleVIDFilter);
DEFINE_FWK_MODULE(HEEPPatEleVIDFilter);
