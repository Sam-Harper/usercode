#include "SHarper/TrigTools/interface/TrigObjP4AnaExample.h"

#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"


#include <iostream>


TrigObjP4AnaExample::TrigObjP4AnaExample(const edm::ParameterSet& iPara)
{
  trigEventTag_ = iPara.getParameter<edm::InputTag>("trigEventTag");
  filterName_ = iPara.getParameter<std::string>("filterName");
  pathName_ = iPara.getParameter<std::string>("pathName");
  trigEventToken_ = consumes<trigger::TriggerEvent>(trigEventTag_);
}

//what we are going to do here is also show an example of how to get the filters of the path automagically 
//without having to specify it manually like we do for filterName_
//this will only work in later MC and runs (around the second half of 2011, cant remember if Fall11 works)
//it is usually better to specify the filters manually for now :)
void TrigObjP4AnaExample::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  bool changed=false;
  hltConfig_.init(run,setup,trigEventTag_.process(),changed);
  std::cout <<"table name "<<hltConfig_.tableName()<<std::endl;
  if(changed) {
    if(hltConfig_.triggerIndex(pathName_)<hltConfig_.size()){ //checks trigger exists
      filtersOfPath_ =hltConfig_.saveTagsModules(pathName_); //hlt config has changed, load the filters used by the path into our vector
    }
  }
}

void TrigObjP4AnaExample::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  //get trigger event
  edm::Handle<trigger::TriggerEvent> trigEvent; 
  iEvent.getByToken(trigEventToken_,trigEvent);
  
  //get the p4s
  std::vector<math::XYZTLorentzVector> trigObjP4s;
  trigtools::getP4sOfObsPassingFilter(trigObjP4s,*trigEvent,filterName_,trigEventTag_.process()); //we get the process name from the trigger event process as they have to be the same...
  
  //do something with them...
  std::cout <<"for filter "<<filterName_<<" nr objects passing "<<trigObjP4s.size()<<std::endl;
  for(size_t objNr=0;objNr<trigObjP4s.size();objNr++){
    std::cout <<"     et obj "<<objNr<<" : "<<trigObjP4s[objNr].Et()<<std::endl;
  }
  

  //now we are going to do the same again for the paths filters
  std::cout <<"for PATH "<<pathName_<<std::endl;
  for(size_t filterNr=0;filterNr<filtersOfPath_.size();filterNr++){
    trigtools::getP4sOfObsPassingFilter(trigObjP4s,*trigEvent,filtersOfPath_[filterNr],trigEventTag_.process()); 
  
    std::cout <<"     filter "<<filtersOfPath_[filterNr]<<" nr objects passing "<<trigObjP4s.size()<<std::endl;
    for(size_t objNr=0;objNr<trigObjP4s.size();objNr++){
      std::cout <<"          et obj "<<objNr<<" : "<<trigObjP4s[objNr].Et()<<std::endl;
    }
  }

}

//define this as a plug-in
DEFINE_FWK_MODULE(TrigObjP4AnaExample);
