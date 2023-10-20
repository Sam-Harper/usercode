#ifndef SHARPER_TRIGTOOLS_MINIAODTRIGGEREXAMPLE
#define SHARPER_TRIGTOOLS_MINIAODTRIGGEREXAMPLE

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/View.h"

#include <vector>
#include <string>
#include <iostream>

//little helper function to get handles easier
//note the use of annoymous namespace to avoid linking conflicts
namespace{
  template<typename T>
  edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token)
  {
    edm::Handle<T> handle;
    iEvent.getByToken(token,handle);
    return handle;
  }
}


//the functions which actually match the trigger objects and see if it passes
namespace{
  std::vector<const pat::TriggerObjectStandAlone*> getMatchedObjs(const float eta,const float phi,const std::vector<pat::TriggerObjectStandAlone>& trigObjs,const float maxDeltaR=0.1)
  {
    std::vector<const pat::TriggerObjectStandAlone*> matchedObjs;
    const float maxDR2 = maxDeltaR*maxDeltaR;
    for(auto& trigObj : trigObjs){
      const float dR2 = reco::deltaR2(eta,phi,trigObj.eta(),trigObj.phi());
      if(dR2<maxDR2) matchedObjs.push_back(&trigObj);
    }
    return matchedObjs;
  }
}

class Ele32DoubleL1ToSingleL1Example : public edm::one::EDAnalyzer<> {
private:

  edm::InputTag trigObjsTag_;
  edm::InputTag trigResultsTag_;
  std::vector<std::string> filtersToPass_;
  std::vector<std::string> pathsToPass_;
  //trigger results stores whether a given path passed or failed
  //a path is series of filters
  edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
  //triggers are the objects the trigger is run on, with a list of filters they pass
  //match to these to see if a given electron/photon/whatever passed a given filter
  edm::EDGetTokenT<std::vector<pat::TriggerObjectStandAlone> > trigObjsToken_;

  edm::EDGetTokenT<edm::View<pat::Electron> > elesToken_;
 
public:
  explicit Ele32DoubleL1ToSingleL1Example(const edm::ParameterSet& iPara);
  ~Ele32DoubleL1ToSingleL1Example(){}
  void beginJob() override;
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)override;

};

Ele32DoubleL1ToSingleL1Example::Ele32DoubleL1ToSingleL1Example(const edm::ParameterSet& iPara):
  trigObjsTag_(iPara.getParameter<edm::InputTag>("trigObjs")),
  trigResultsTag_(iPara.getParameter<edm::InputTag>("trigResults")),
  trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
  trigObjsToken_(consumes<std::vector<pat::TriggerObjectStandAlone> >(trigObjsTag_)),
  elesToken_(consumes<edm::View<pat::Electron> >(iPara.getParameter<edm::InputTag>("eles")))
{
  
}

void Ele32DoubleL1ToSingleL1Example::beginJob()
{

}



void Ele32DoubleL1ToSingleL1Example::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 

  auto trigResultsHandle = getHandle(iEvent,trigResultsToken_) ;
  auto trigObjsHandle = getHandle(iEvent,trigObjsToken_); 
  auto elesHandle = getHandle(iEvent,elesToken_);
  
  //so the filter names are all packed in miniAOD so we need to create a new collection of them which are unpacked  
  std::vector<pat::TriggerObjectStandAlone> unpackedTrigObjs;
  for(auto& trigObj : *trigObjsHandle){
    unpackedTrigObjs.push_back(trigObj);
    unpackedTrigObjs.back().unpackFilterLabels(iEvent,*trigResultsHandle);
  }
  std::cout <<"checking eles "<<std::endl;
  for(auto& ele : *elesHandle){
    //the eta/phi of e/gamma trigger objects is the supercluster eta/phi
    const float eta = ele.superCluster()->eta();
    const float phi = ele.superCluster()->phi();
    
    //now match ALL objects in a cone of DR<0.1
    //it is important to match all objects as there are different ways to reconstruct the same electron
    //eg, L1 seeded, unseeded, as a jet etc
    //and so you want to be sure you get all possible objects
    std::vector<const pat::TriggerObjectStandAlone*> matchedTrigObjs = getMatchedObjs(eta,phi,unpackedTrigObjs,0.1);
    for(const auto trigObj : matchedTrigObjs){
      //now just check if it passes the two filters
      if(trigObj->hasFilterLabel("hltEle32L1DoubleEGWPTightGsfTrackIsoFilter") &&
	 trigObj->hasFilterLabel("hltEGL1SingleEGOrFilter") ) {
	std::cout <<" ele "<<ele.et()<<" "<<eta<<" "<<phi<<" passes HLT_Ele32_WPTight_Gsf"<<std::endl;
      }
    }
    
  }

}



DEFINE_FWK_MODULE(Ele32DoubleL1ToSingleL1Example);
#endif
