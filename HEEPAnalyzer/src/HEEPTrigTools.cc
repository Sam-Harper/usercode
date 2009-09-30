#include "SHarper/HEEPAnalyzer/interface/HEEPTrigTools.h"

#include "FWCore/ParameterSet/interface/Registry.h"

heep::TrigCodes::TrigBitSet heep::trigtools::getHLTFiltersPassed(const std::vector<std::pair<std::string,int> >& filters,edm::Handle<trigger::TriggerEvent> trigEvt,const std::string& hltTag)
{
  heep::TrigCodes::TrigBitSet evtTrigs;
  for(size_t filterNrInVec=0;filterNrInVec<filters.size();filterNrInVec++){
    size_t filterNrInEvt = trigEvt->filterIndex(edm::InputTag(filters[filterNrInVec].first,"",hltTag).encode());
    const heep::TrigCodes::TrigBitSet filterCode = TrigCodes::getCode(filters[filterNrInVec].first.c_str());
    if(filterNrInEvt<trigEvt->sizeFilters()){ //filter found in event, something *may* pass it
      const trigger::Keys& trigKeys = trigEvt->filterKeys(filterNrInEvt);
      if(static_cast<int>(trigKeys.size())>=filters[filterNrInVec].second){
	evtTrigs |=filterCode; //if the required number of objects pass add it as passed
      }
    }//end check if filter is present
  }//end loop over all filters

  return evtTrigs;
 
}

//I have the horrible feeling that I'm converting into an intermediatry format and then coverting back again
//Okay how this works
//1) create a TrigBitSet for each particle set to 0 initally
//2) loop over each filter, for each particle that passes the filter, set the appropriate bit in the TrigBitSet
//3) after that, loop over each particle setting the its TrigBitSet which has been calculated
void heep::trigtools::setHLTFiltersObjPasses(std::vector<heep::Ele>& particles,const std::vector<std::string>& filters,edm::Handle<trigger::TriggerEvent> trigEvt,const std::string& hltTag,const double maxDeltaR,const double maxPtDiffRel)
{
  std::vector<heep::TrigCodes::TrigBitSet> partTrigBits(particles.size());
  for(size_t filterNrInVec=0;filterNrInVec<filters.size();filterNrInVec++){
    size_t filterNrInEvt = trigEvt->filterIndex(edm::InputTag(filters[filterNrInVec],"",hltTag).encode());
    const heep::TrigCodes::TrigBitSet filterCode = heep::TrigCodes::getCode(filters[filterNrInVec].c_str());

    if(filterNrInEvt<trigEvt->sizeFilters()){ //filter found in event, something *may* pass it
      const trigger::Keys& trigKeys = trigEvt->filterKeys(filterNrInEvt);  //trigger::Keys is actually a vector<uint16_t> holding the position of trigger objects in the trigger collection passing the filter
      const trigger::TriggerObjectCollection & trigObjColl(trigEvt->getObjects());
      for(size_t partNr=0;partNr<particles.size();partNr++){
	for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){
	  float trigObjEta = trigObjColl[*keyIt].eta();
	  float trigObjPhi = trigObjColl[*keyIt].phi();
	  float trigObjPt = trigObjColl[*keyIt].et();
	  if (reco::deltaR(particles[partNr].eta(),particles[partNr].phi(),trigObjEta,trigObjPhi) < maxDeltaR &&
	      fabs(trigObjPt-particles[partNr].et())/particles[partNr].et()<maxPtDiffRel){
	    partTrigBits[partNr] |= filterCode;
	  }//end dR match trig obj match test
	}//end loop over all objects passing filter
      }//end loop over particles
    }//end check if filter is present
  }//end loop over all filters

  for(size_t partNr=0;partNr<particles.size();partNr++) particles[partNr].setTrigBits(partTrigBits[partNr]);

}

//this function runs over all parameter sets for every module that has ever run on an event in this job
//it looks for the specified filter module
//and returns the minimum number of objects required to pass the filter, -1 if its not found
//which is either the ncandcut or MinN parameter in the filter config
//assumption: nobody will ever change MinN or ncandcut without changing the filter name
//as this just picks the first module name and if 2 different versions of HLT were run with the filter having
//a different min obj required in the two versions, this may give the wrong answer
int heep::trigtools::getMinNrObjsRequiredByFilter(const std::string& filterName)
{
 
  //will return out of for loop once its found it to save time
  const edm::pset::Registry* psetRegistry = edm::pset::Registry::instance();
  if(psetRegistry==NULL) return -1;
  for(edm::pset::Registry::const_iterator psetIt=psetRegistry->begin();psetIt!=psetRegistry->end();++psetIt){ //loop over every pset for every module ever run
    const std::map<std::string,edm::Entry>& mapOfPara  = psetIt->second.tbl(); //contains the parameter name and value for all the parameters of the pset
    const std::map<std::string,edm::Entry>::const_iterator itToModLabel = mapOfPara.find("@module_label"); 
    if(itToModLabel!=mapOfPara.end()){
      if(itToModLabel->second.getString()==filterName){ //moduleName is the filter name, we have found filter, we will now return something
	std::map<std::string,edm::Entry>::const_iterator itToCandCut = mapOfPara.find("ncandcut");
	if(itToCandCut!=mapOfPara.end() && itToCandCut->second.typeCode()=='I') return itToCandCut->second.getInt32();
	else{ //checks if MinN exists and is int32, if not return -1
	  itToCandCut = mapOfPara.find("MinN");
	  if(itToCandCut!=mapOfPara.end() && itToCandCut->second.typeCode()=='I') return itToCandCut->second.getInt32();
	  else return -1;
	}
      }
      
    }
  }
  return -1;
}
