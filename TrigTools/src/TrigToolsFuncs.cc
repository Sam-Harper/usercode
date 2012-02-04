#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"

void trigtools::getP4sOfObsPassingFilter(std::vector<math::XYZTLorentzVector>& p4s,const trigger::TriggerEvent& trigEvent,const std::string& filterName,const std::string& hltProcess)
{
  p4s.clear();

  edm::InputTag filterTag(filterName,"",hltProcess); 
  trigger::size_type filterIndex = trigEvent.filterIndex(filterTag); 
  if(filterIndex<trigEvent.sizeFilters()){ //check that filter is in triggerEvent
    const trigger::Keys& trigKeys = trigEvent.filterKeys(filterIndex); 
    const trigger::TriggerObjectCollection & trigObjColl(trigEvent.getObjects());
    for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){ 
      const trigger::TriggerObject& obj = trigObjColl[*keyIt];
      math::XYZTLorentzVector objP4;
      objP4.SetPxPyPzE(obj.px(),obj.py(),obj.pz(),obj.energy());
      p4s.push_back(objP4);
    }//end loop over keys
  }//end check that filter is valid and in trigEvent
}

void trigtools::getP4sOfObsPassingFilter(std::vector<TLorentzVector>& p4s,const trigger::TriggerEvent& trigEvent,const std::string& filterName,const std::string& hltProcess)
{
  p4s.clear();
 
  edm::InputTag filterTag(filterName,"",hltProcess); 
  trigger::size_type filterIndex = trigEvent.filterIndex(filterTag); 
  if(filterIndex<trigEvent.sizeFilters()){ //check that filter is in triggerEvent
    const trigger::Keys& trigKeys = trigEvent.filterKeys(filterIndex); 
    const trigger::TriggerObjectCollection & trigObjColl(trigEvent.getObjects());
    for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){ 
      const trigger::TriggerObject& obj = trigObjColl[*keyIt];
      TLorentzVector objP4;
      objP4.SetPtEtaPhiM(obj.pt(),obj.eta(),obj.phi(),obj.mass());
      p4s.push_back(objP4);
    }//end loop over keys
  }//end check that filter is valid and in trigEvent
}


