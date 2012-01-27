#include "SHarper/TrigTools/interface/HEEPHLTWP80TrigEffAnalyser.h"

#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include <iostream>


HEEPHLTWP80TrigEffAnalyser::HEEPHLTWP80TrigEffAnalyser(const edm::ParameterSet& iPara)
{
  trigEventTag_ = iPara.getParameter<edm::InputTag>("trigEventTag");
  tagFilterName_ = iPara.getParameter<std::string>("tagFilterName");
  probeFilterName_ =  iPara.getParameter<std::string>("probeFilterName");
  wp80FilterName_ =  iPara.getParameter<std::string>("wp80FilterName");
}


void HEEPHLTWP80TrigEffAnalyser::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  bool changed=false;
  hltConfig_.init(run,setup,trigEventTag_.process(),changed);
  std::cout <<"table name "<<hltConfig_.tableName()<<std::endl;
 //  if(changed) {
//     if(hltConfig_.triggerIndex(pathName_)<hltConfig_.size()){ //checks trigger exists
//       filtersOfPath_ =hltConfig_.saveTagsModules(pathName_); //hlt config has changed, load the filters used by the path into our vector
//     }
//   }
}

void HEEPHLTWP80TrigEffAnalyser::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  //get trigger event
  edm::Handle<trigger::TriggerEvent> trigEvent; 
  iEvent.getByLabel(trigEventTag_,trigEvent);
  
  //get the p4s
  std::vector<math::XYZTLorentzVector> tagP4s;
  trigtools::getP4sOfObsPassingFilter(tagP4s,*trigEvent,tagFilterName_,trigEventTag_.process());
  std::vector<math::XYZTLorentzVector> probeP4s;
  trigtools::getP4sOfObsPassingFilter(probeP4s,*trigEvent,probeFilterName_,trigEventTag_.process());
  std::vector<math::XYZTLorentzVector> wp80P4s;
  trigtools::getP4sOfObsPassingFilter(wp80P4s,*trigEvent,wp80FilterName_,trigEventTag_.process());
  

  // std::cout <<"nr tags "<<tagP4s.size()<<std::endl;
//   std::cout <<"nr probes "<<probeP4s.size()<<std::endl;
//   std::cout <<"nr wp80s "<<wp80P4s.size()<<std::endl;
  

  edm::Handle<reco::GsfElectronCollection> eleHandle; 
  iEvent.getByLabel("gsfElectrons",eleHandle);
  
  edm::Handle<edm::ValueMap<int> > heepBitHandle;
  iEvent.getByLabel("heepId",heepBitHandle);
  
  std::vector<const reco::GsfElectron*> heepEles;
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    reco::GsfElectronRef gsfRef(eleHandle,eleNr);
    if((*heepBitHandle)[gsfRef]==0x0) heepEles.push_back(&*gsfRef);
     
  }
  //you now have p4 of the tags, the probes and wp80s. Match to the HEEP electrons and you're done

}

//define this as a plug-in
DEFINE_FWK_MODULE(HEEPHLTWP80TrigEffAnalyser);
