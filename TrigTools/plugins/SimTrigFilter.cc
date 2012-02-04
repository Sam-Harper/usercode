#include "SHarper/TrigTools/interface/SimTrigFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include <functional>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
//little helper func to convert a string to whatever 
template <class T>
bool fromString(T& t, 
		const std::string& s, 
		std::ios_base& (*f)(std::ios_base&)=std::dec)
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}


SimTrigFilter:: SimTrigFilter(const edm::ParameterSet& iPara)
{
  trigEventTag_ = iPara.getParameter<edm::InputTag>("trigEventTag");
  
  std::vector<std::string> cuts = iPara.getParameter<std::vector<std::string> >("trigCuts");
  
  for(size_t cutNr=0;cutNr<cuts.size();cutNr++){
    TrigCut cut(cuts[cutNr],trigEventTag_.process());
    trigCuts_.push_back(cut);
  }

}

bool SimTrigFilter::filter(edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<trigger::TriggerEvent> trigEvtHandle;
  iEvent.getByLabel(trigEventTag_,trigEvtHandle);

  
  //counts the number of cuts failing and requires them to be zero
  return std::count_if(trigCuts_.begin(),trigCuts_.end(),
		       std::not1(std::bind2nd(std::mem_fun_ref(&TrigCut::passCut),&*trigEvtHandle)))==0;
}

  


SimTrigFilter::TrigCut::TrigCut(const std::string& cut,const std::string& processName):
  filterName_(),
  processName_(processName),
  ptCut_(-1),
  nrObjs_(-1)
{
  std::vector<std::string> splitStrings;
  boost::split(splitStrings,cut,boost::is_any_of(std::string(":")));
  
  if(splitStrings.size()!=3 || 
     !fromString(filterName_,splitStrings[0]) || 
     !fromString(ptCut_,splitStrings[1]) ||
     !fromString(nrObjs_,splitStrings[2])){
    edm::LogError("SimTrigFilter")<<" Error cut string "<<cut<<" not of format filterName:ptCut:nrObjs ";
  }
  
}

bool SimTrigFilter::TrigCut::passCut(const trigger::TriggerEvent* trigEvent)const
{
  edm::InputTag filterTag(filterName_,"",processName_);
  int nrPassing=0;
  trigger::size_type filterIndex = trigEvent->filterIndex(filterTag); 
  if(filterIndex<trigEvent->sizeFilters()){
    const trigger::Keys& trigKeys = trigEvent->filterKeys(filterIndex); 
    const trigger::TriggerObjectCollection & trigObjColl(trigEvent->getObjects());
    for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){ 
      const trigger::TriggerObject& obj = trigObjColl[*keyIt];
      if(obj.pt()>ptCut_) nrPassing++;
    }
  }

 
  if(nrPassing>=nrObjs_) return true;
  else return false;
   

}

//define this as a plug-in
DEFINE_FWK_MODULE(SimTrigFilter);
