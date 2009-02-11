#include "SHarper/HEEPAnalyzer/interface/HEEPTrigTools.h"



heep::TrigCodes::TrigBitSet heep::trigtools::getHLTFiltersPassed(const std::vector<std::string>& filters,edm::Handle<trigger::TriggerEvent> trigEvt,const std::string& hltTag)
{
  heep::TrigCodes::TrigBitSet evtTrigs;
  for(size_t filterNrInVec=0;filterNrInVec<filters.size();filterNrInVec++){
    size_t filterNrInEvt = trigEvt->filterIndex(edm::InputTag(filters[filterNrInVec],"",hltTag).encode());
    const heep::TrigCodes::TrigBitSet filterCode = TrigCodes::getCode(filters[filterNrInVec].c_str());
    if(filterNrInEvt<trigEvt->sizeFilters()){ //filter found in event, something passes it
      evtTrigs |=filterCode; //if something passes it add to the event trigger bits
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

    if(filterNrInEvt<trigEvt->sizeFilters()){ //filter found in event, something passes it
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
	  }//end dR<0.3 trig obj match test
	}//end loop over all objects passing filter
      }//end loop over particles
    }//end check if filter is present
  }//end loop over all filters

  for(size_t partNr=0;partNr<particles.size();partNr++) particles[partNr].setTrigBits(partTrigBits[partNr]);

}
