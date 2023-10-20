#ifndef SHARPER_TRIGTOOLS_MINIAODTRIGGEREXAMPLE
#define SHARPER_TRIGTOOLS_MINIAODTRIGGEREXAMPLE

#include "FWCore/Framework/interface/stream/EDAnalyzer.h"
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
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Common/interface/View.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"

#include <vector>
#include <string>
#include <iostream>
#include <regex>

//a little example to do trigger matching for muons
//this also shows how to access only those trigger filters in the datasets you are interested in 


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

  bool checkFilters(const std::vector<pat::TriggerObjectStandAlone>& trigObjs,const std::vector<std::string>& filterNames)
  {
    bool passAnyFilter=false;
    
    for(const auto& trigObj : trigObjs){
      //normally would auto this but to make it clearer for the example
      const std::vector<std::string>& objFilters = trigObj.filterLabels();
      for(auto& filterName : filterNames){


	//I dont think filterLabels are sorted so use std::find to see if filterName is in 
	//the list of passed filters for this object
	if(std::find(objFilters.begin(),objFilters.end(),filterName)!=objFilters.end()){
	  std::cout <<" object "<<trigObj.eta()<<" "<<trigObj.phi()<<" passes "<<filterName<<std::endl;
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
  
  //strip version name
  //it is useful to strip version number off the end of trigger paths 
  //as it serves very little purpose and doing so makes it easier to copmare
  std::string stripHLTVersionNr(const std::string& pathName){
    auto versionIndx = pathName.rfind("_v");
    return pathName.substr(0,versionIndx+2);
  }


}


class MiniAODTriggerMuonExample : public edm::stream::EDAnalyzer<> {
private: 
  edm::InputTag trigResultsTag_;
  //we are only interseted in  the paths/filters of these datasets
  std::vector<std::string> datasets_;
  std::vector<std::string> filtersToCheck_;
  std::vector<std::string> paths_;
  
  //this allows us to match paths to datasets
  HLTPrescaleProvider hltConfig_;


  //trigger results stores whether a given path passed or failed
  //a path is series of filters
  edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
  //triggers are the objects the trigger is run on, with a list of filters they pass
  //match to these to see if a given electron/photon/whatever passed a given filter
  edm::EDGetTokenT<std::vector<pat::TriggerObjectStandAlone> > trigObjsToken_;

  edm::EDGetTokenT<edm::View<pat::Muon> > muonsToken_;
 
public:
  explicit MiniAODTriggerMuonExample(const edm::ParameterSet& iPara);
  ~MiniAODTriggerMuonExample(){}
  void beginRun(const edm::Run& run,const edm::EventSetup& setup) override;
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)override;

};

MiniAODTriggerMuonExample::MiniAODTriggerMuonExample(const edm::ParameterSet& iPara):
  trigResultsTag_(iPara.getParameter<edm::InputTag>("trigResults")),
  datasets_(iPara.getParameter<std::vector<std::string> >("datasets")), 
  hltConfig_(iPara,consumesCollector(),*this),
  trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
  trigObjsToken_(consumes<std::vector<pat::TriggerObjectStandAlone> >(iPara.getParameter<edm::InputTag>("trigObjs"))),
  muonsToken_(consumes<edm::View<pat::Muon> >(iPara.getParameter<edm::InputTag>("muons")))
{
  
}



void MiniAODTriggerMuonExample::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  bool changed=false;
  hltConfig_.init(run,setup,trigResultsTag_.process(),changed);
  if(changed) {
    //now we populate all filters of the paths in the datasets specificed
    //and also the paths we want
    filtersToCheck_.clear();
    paths_.clear();
    std::set<std::string> filtersSet;
    for(const std::string& dataset : datasets_){
      for(const std::string& path : hltConfig_.hltConfigProvider().datasetContent(dataset)){
	paths_.push_back(path);
	for(const std::string& filter : hltConfig_.hltConfigProvider().saveTagsModules(path)){
	  filtersSet.insert(filter);
	}
      }
    }
    filtersToCheck_.reserve(filtersSet.size());
    filtersToCheck_.assign(filtersSet.begin(),filtersSet.end());
    std::sort(filtersToCheck_.begin(),filtersToCheck_.end());
  }
}

void MiniAODTriggerMuonExample::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 

  auto trigResultsHandle = iEvent.getHandle(trigResultsToken_);
  auto trigObjsHandle = iEvent.getHandle(trigObjsToken_); 
  auto muonsHandle = iEvent.getHandle(muonsToken_);
  
  const edm::TriggerNames& trigNames = iEvent.triggerNames(*trigResultsHandle);
  for(auto& pathName : paths_){
    size_t pathIndex = getPathIndex(pathName,trigNames);
    std::string pathNameWOVer = stripHLTVersionNr(pathName);
    if(pathIndex>=trigNames.size()) std::cout <<" path "<<pathNameWOVer<<" not found in menu"<<std::endl;
    else{
      std::cout <<" path index "<<pathIndex << " "<<trigNames.triggerName(pathIndex)<<std::endl;
      if(trigResultsHandle->accept(pathIndex)) std::cout <<" path "<<pathNameWOVer<<" passed"<<std::endl;
      else std::cout <<" path "<<pathNameWOVer<<" failed"<<std::endl;
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

  //prescale column (only changes every lumisection)
  int prescaleColumn = hltConfig_.prescaleSet(iEvent,iSetup);
  std::cout <<"prescale column "<<prescaleColumn<<std::endl;


  std::cout <<"checking muons "<<std::endl;
  for(auto& muon : *muonsHandle){
    checkFilters(muon.eta(),muon.phi(),trigObjsUnpacked,filtersToCheck_);
  }
  std::cout <<"checking filters"<<std::endl;
  checkFilters(trigObjsUnpacked,filtersToCheck_);
}



DEFINE_FWK_MODULE(MiniAODTriggerMuonExample);
#endif
