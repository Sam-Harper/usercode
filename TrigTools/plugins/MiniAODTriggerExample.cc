#ifndef SHARPER_TRIGTOOLS_MINIAODTRIGGEREXAMPLE
#define SHARPER_TRIGTOOLS_MINIAODTRIGGEREXAMPLE

#include "FWCore/Framework/interface/EDAnalyzer.h"
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

  bool checkFilters(const float eta,const float phi,const std::vector<pat::TriggerObjectStandAlone>& trigObjs,const std::vector<std::string>& filterNames,const float maxDeltaR=0.1)
  {
    bool passAnyFilter=false;
    const auto matchedObjs = getMatchedObjs(eta,phi,trigObjs,maxDeltaR);
    for(auto& filterName : filterNames){
      for(const auto trigObj : matchedObjs){
	//normally would auto this but to make it clearer for the example
	const std::vector<std::string>& objFilters = trigObj->filterLabels();
	//I dont think filterLabels are sorted so use std::find to see if filterName is in 
	//the list of passed filters for this object
	if(std::find(objFilters.begin(),objFilters.end(),filterName)!=objFilters.end()){
	  std::cout <<" object "<<eta<<" "<<phi<<" passes "<<filterName<<std::endl;
	  passAnyFilter=true;
	}
      }//end loop over matched trigger objects
    }//end loop over filter lables
    return passAnyFilter;
  }

  //this function determines the index of the path in trigger results, if not
  //found it returns an index equal to the size of triggerNames
  //note it matches on whether the name in triggernames starts with the pathName
  //this is because HLT paths are of form HLT_TriggerName_vX where X is the version number
  //X changes frequiently so often you want to match all versions which can be
  //achieved by passing in HLT_TriggerName_v to this function 
  size_t getPathIndex(const std::string& pathName,const edm::TriggerNames& trigNames){
    for(size_t index = 0;index<trigNames.size(); index++){
      if(trigNames.triggerName(index).find(pathName)==0){
	return index;
      }
    }
    return trigNames.size();
  }

}


class MiniAODTriggerExample : public edm::EDAnalyzer {
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
  explicit MiniAODTriggerExample(const edm::ParameterSet& iPara);
  ~MiniAODTriggerExample(){}
  void beginJob() override;
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)override;

};

MiniAODTriggerExample::MiniAODTriggerExample(const edm::ParameterSet& iPara):
  trigObjsTag_(iPara.getParameter<edm::InputTag>("trigObjs")),
  trigResultsTag_(iPara.getParameter<edm::InputTag>("trigResults")),
  filtersToPass_(iPara.getParameter<std::vector<std::string> >("filtersToPass")), 
  pathsToPass_(iPara.getParameter<std::vector<std::string> >("pathsToPass")),
  trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
  trigObjsToken_(consumes<std::vector<pat::TriggerObjectStandAlone> >(trigObjsTag_)),
  elesToken_(consumes<edm::View<pat::Electron> >(iPara.getParameter<edm::InputTag>("eles")))
{
  
}

void MiniAODTriggerExample::beginJob()
{

}



void MiniAODTriggerExample::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 

  auto trigResultsHandle = getHandle(iEvent,trigResultsToken_) ;
  auto trigObjsHandle = getHandle(iEvent,trigObjsToken_); 
  auto elesHandle = getHandle(iEvent,elesToken_);
  
  const edm::TriggerNames& trigNames = iEvent.triggerNames(*trigResultsHandle);
  std::cout <<"checking paths "<<std::endl;
  for(auto& pathName : pathsToPass_){
    size_t pathIndex = getPathIndex(pathName,trigNames);
    if(pathIndex>=trigNames.size()) std::cout <<" path "<<pathName<<" not found in menu"<<std::endl;
    else{
      std::cout <<" path index "<<pathIndex << " "<<trigNames.triggerName(pathIndex)<<std::endl;
      if(trigResultsHandle->accept(pathIndex)) std::cout <<" path "<<pathName<<" passed"<<std::endl;
      else std::cout <<" path "<<pathName<<" failed"<<std::endl;
    }
  }
 
  //now we will look at the filters passed
  //before we do this we need to make a new collection of trig objects
  //with their filters unpacked
  //we have to make a new copy as the unpacking modifies them and CMSSW
  //forbids (for very good reasons) modification of products in the event
  std::vector<pat::TriggerObjectStandAlone> trigObjsUnpacked;
  for(auto& trigObj : *trigObjsHandle){
    trigObjsUnpacked.push_back(trigObj);
    trigObjsUnpacked.back().unpackFilterLabels(iEvent,*trigResultsHandle);
  }


  std::cout <<"checking eles "<<std::endl;
  for(auto& ele : *elesHandle){
    checkFilters(ele.superCluster()->eta(),ele.superCluster()->phi(),trigObjsUnpacked,filtersToPass_);
  }

}



DEFINE_FWK_MODULE(MiniAODTriggerExample);
#endif
