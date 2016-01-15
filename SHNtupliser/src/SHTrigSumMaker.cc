
#include "SHarper/SHNtupliser/interface/SHTrigSumMaker.h"
#include "SHarper/SHNtupliser/interface/LogErr.hh"
#include "SHarper/SHNtupliser/interface/SHTrigSummary.hh"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"

#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Common/interface/TriggerNames.h"

void SHTrigSumMaker::makeSHTrigSum(const heep::Event& heepEvent,SHTrigSummary& shTrigSum)
{
  if(heepEvent.handles().trigEvent.isValid()){
    const trigger::TriggerEvent& trigEvt = heepEvent.triggerEvent();
    
    const edm::TriggerResults& trigResults = *heepEvent.handles().trigResults;
    const edm::TriggerNames& trigNames = heepEvent.event().triggerNames(trigResults);  
 
    makeSHTrigSum(trigEvt,trigResults,trigNames,
		  heepEvent.hltConfig(),heepEvent.event(),heepEvent.eventSetup(),
		  shTrigSum);
  }
}


void SHTrigSumMaker::makeSHTrigSum(const trigger::TriggerEvent& trigEvt,
				   const edm::TriggerResults& trigResults,
				   const edm::TriggerNames& trigNames,
				   const HLTConfigProvider& hltConfig,
				   const edm::Event& edmEvent,
				   const edm::EventSetup& edmEventSetup,
				   SHTrigSummary& shTrigSum)
				
{
  if(!hltConfig.inited()){
    throw edm::Exception(edm::errors::LogicError,"Error, HLTConfig is not initialised, this will end poorly");
  }
  
  //there is a small potenial bug here if the menu changes but is kept the same name
  //this can only happen for private production/studies and we will ignore this
  if(hltConfig.tableName()!=shTrigSum.menuName()){
    shTrigSum.clearMenu();
    fillMenu_(shTrigSum,hltConfig);
  }
  shTrigSum.setProcessName(hltConfig.processName()); //in theory this could change without chaning menu name and its fine
  //  std::cout <<"menu "<<shTrigSum.menuName()<<std::endl;
  shTrigSum.clearEvent();
 
  shTrigSum.setPreScaleColumn(hltConfig.prescaleSet(edmEvent,edmEventSetup));
			      
  fillSHTrigResults_(trigResults,trigNames,hltConfig,edmEvent,edmEventSetup,shTrigSum);
  fillSHTrigObjs_(trigEvt,shTrigSum);
  shTrigSum.sort();
}


void SHTrigSumMaker::fillMenu_(SHTrigSummary& shTrigSum,
			       const HLTConfigProvider& hltConfig)
{

  shTrigSum.setProcessName(hltConfig.processName());
  shTrigSum.setGlobalTag(hltConfig.globalTag());
  shTrigSum.setMenuName(hltConfig.tableName());
  
  std::set<std::string> pathNames;
  for(auto& pathName : hltConfig.triggerNames()){
    pathNames.insert(rmTrigVersionFromName(pathName));
  }

  
  
  std::set<std::string> filterNames;    
  for(size_t trigNr=0;trigNr<hltConfig.size();trigNr++){
    const std::vector<std::string> savedMods = hltConfig.saveTagsModules(trigNr);
    for(const std::string& savedMod : savedMods){
      filterNames.insert(savedMod);
      //   std::cout <<"saved mod "<<savedMod<<std::endl;
    }
    // for(const std::string& mod : hltConfig.moduleLabels(trigNr)){

    //   std::cout <<"mod "<<mod<<std::endl;
    // }
  }
  
  shTrigSum.setPathBitsDef(pathNames);
  shTrigSum.setFilterBitsDef(filterNames);

}

void SHTrigSumMaker::fillSHTrigResults_(const edm::TriggerResults& trigResults,
					const edm::TriggerNames& trigNames,
					const HLTConfigProvider& hltConfig,
					const edm::Event& edmEvent,
					const edm::EventSetup& edmEventSetup,
					SHTrigSummary& shTrigSum)
					
{
  if(trigNames.size()!=trigResults.size()){
    std::ostringstream msg;
    msg<<" Error trig results and trig names not the same size "<<trigNames.size()<<" "<<trigResults.size();
    throw edm::Exception(edm::errors::LogicError,msg.str());
  }

  for(size_t pathNr=0;pathNr<trigResults.size();pathNr++){
    
    SHTrigResult trigResult;
    
    const std::string& pathName = trigNames.triggerName(pathNr);
    const std::string  pathNameWOVersion=rmTrigVersionFromName(pathName);
    size_t bitNr=shTrigSum.pathBitsDef().getBitNr(pathNameWOVersion);
    //    std::cout <<"bitNr "<<bitNr<<" "<<pathNameWOVersion<<std::endl;
    int prescale=hltConfig.inited() ? hltConfig.prescaleValue(edmEvent,edmEventSetup,pathName) : -1;
    SHTrigResult::Status status = static_cast<SHTrigResult::Status>(trigResults[pathNr].state());
    
    shTrigSum.addTrigResult(SHTrigResult(bitNr,status,prescale));     
  } 
}

void SHTrigSumMaker::fillSHTrigObjs_(const trigger::TriggerEvent& trigEvt,
				     SHTrigSummary& shTrigSum)
{

  //so we are going to make a temporary vector of all TriggerObjs
  //we are then loop over all the filters and set bits on the objs it passes
  //we also get the ID of the object at this point
  //we then convert the temporary objects into the final objects
  //the temporary stage is because we cant modifiy the type or bits passes of the object once its 
  //created which is intentional...

  std::vector<std::pair<P4Struct,std::pair<TBits,int> > > shTrigObjsTmp;
  const trigger::TriggerObjectCollection & trigObjs = trigEvt.getObjects();
  for(auto trigObj : trigObjs){
    shTrigObjsTmp.push_back({{trigObj.pt(),trigObj.eta(),trigObj.phi(),trigObj.mass()},
	  {TBits(),0}});
  }

  for(size_t filterNr=0;filterNr<trigEvt.sizeFilters();filterNr++){
    const trigger::Keys& trigKeys = trigEvt.filterKeys(filterNr);  //trigger::Keys is actually a vector<uint16_t> holding the position of trigger objects in the trigger collection passing the filter
    const trigger::Vids& trigIds = trigEvt.filterIds(filterNr);

    if(trigKeys.size()!=trigIds.size()){
      std::ostringstream msg;
      msg<<" Error, for filter  "<<trigEvt.filterTag(filterNr).label()<<" keys and ids sizes do not match "<<trigKeys.size()<<" "<<trigIds.size()<<std::endl;
      throw edm::Exception(edm::errors::LogicError,msg.str());
    }


    size_t bitNr=shTrigSum.filterBitsDef().getBitNr(trigEvt.filterTag(filterNr).label());
    //std::cout <<"bit Nr "<<bitNr<<" "<< trigEvt.filterTag(filterNr).label()<<" nr pass "<<trigKeys.size()<<std::endl;
    for(size_t objNr=0;objNr<trigKeys.size();objNr++){
      auto key = trigKeys[objNr];
      auto id = convertToSHTrigType(trigIds[objNr]);
      shTrigObjsTmp[key].second.first.SetBitNumber(bitNr);
      if(shTrigObjsTmp[key].second.second==0) shTrigObjsTmp[key].second.second=id;
      else if(shTrigObjsTmp[key].second.second!=id && id!=0){
	LogErr<<" Error trigger object type for filter "<<trigEvt.filterTag(filterNr).label()<<" was previously type "<<shTrigObjsTmp[key].second.second<<" but is now type "<<id<<" cmss id "<<trigIds[objNr]<<std::endl;
      }
    }
  }
  
  for(const auto& trigObj : shTrigObjsTmp){
    if(trigObj.second.first.CountBits()!=0){
      shTrigSum.addTrigObj(SHTrigObj(trigObj.first.pt,trigObj.first.eta,
				     trigObj.first.phi,trigObj.first.mass,
				     trigObj.second.second,
				     trigObj.second.first));
    }
    
  }
}    
    
int SHTrigSumMaker::convertToSHTrigType(int cmsswTrigType) 
{
  switch(cmsswTrigType){
  case trigger::TriggerL1Mu: return SHTrigObj::L1S1MUON;
  case trigger::TriggerL1NoIsoEG: return SHTrigObj::L1S1EGNONISO;
  case trigger::TriggerL1IsoEG: return SHTrigObj::L1S1EGISO;
  case trigger::TriggerL1CenJet: return SHTrigObj::L1S1JET;
  case trigger::TriggerL1ForJet: return SHTrigObj::L1S1JET;
  case trigger::TriggerL1TauJet: return SHTrigObj::L1S1TAU;
  case trigger::TriggerL1ETM: return SHTrigObj::L1S1SUM;
  case trigger::TriggerL1ETT: return SHTrigObj::L1S1SUM;
  case trigger::TriggerL1HTT: return SHTrigObj::L1S1SUM;
  case trigger::TriggerL1HTM: return SHTrigObj::L1S1SUM;
  case trigger::TriggerL1JetCounts: return SHTrigObj::L1S1MISC;
  case trigger::TriggerL1HfBitCounts: return SHTrigObj::L1S1MISC;
  case trigger::TriggerL1HfRingEtSums: return SHTrigObj::L1S1MISC;
  case trigger::TriggerL1TechTrig: return SHTrigObj::L1S1MISC;
  case trigger::TriggerL1Castor: return SHTrigObj::L1S1MISC;
  case trigger::TriggerL1BPTX: return SHTrigObj::L1S1MISC;
  case trigger::TriggerL1GtExternal: return SHTrigObj::L1S1MISC;
  case trigger::TriggerPhoton: return SHTrigObj::PHOTON;
  case trigger::TriggerElectron: return SHTrigObj::ELECTRON;
  case trigger::TriggerMuon: return SHTrigObj::MUON;
  case trigger::TriggerTau: return SHTrigObj::TAU;
  case trigger::TriggerJet: return SHTrigObj::JET;
  case trigger::TriggerBJet: return SHTrigObj::JET;
  case trigger::TriggerMET: return SHTrigObj::SUM;
  case trigger::TriggerTET: return SHTrigObj::SUM;
  case trigger::TriggerTHT: return SHTrigObj::SUM;
  case trigger::TriggerMHT: return SHTrigObj::SUM;
  case trigger::TriggerTrack: return SHTrigObj::MISC;
  case trigger::TriggerCluster: return SHTrigObj::PHOTON;
  case trigger::TriggerMETSig: return SHTrigObj::SUM;
  case trigger::TriggerELongit: return SHTrigObj::MISC;
  case trigger::TriggerMHTSig: return SHTrigObj::SUM;
  case trigger::TriggerHLongit: return SHTrigObj::MISC;
  default: return SHTrigObj::UNDEFINED;
  }
}

std::string SHTrigSumMaker::rmTrigVersionFromName(std::string pathName)
{
  size_t versionIndex = pathName.rfind("_v");
  
  if(versionIndex!=std::string::npos) pathName.erase(versionIndex+2);
  return pathName;
}


// void SHTrigSumMaker::fillFilterTypeCodes(const HLTConfigProvider& config)
// {
//   for(size_t trigNr=0;trigNr<config.size();trigNr++){
//     const std::vector<std::string> savedMods = config.saveTagsModules(trigNr);
//     for(const std::string& savedMod : savedMods){
//       auto elem = savedMods.insert({savedMod,0});
//       if(elem.second){//first insert
// 	elem.first->second=getFilterTypeCode(elem.first->first);
//       }
	
//     }
//   }

//   for(auto& filterTypeCode : filterNamesToTypeCodes_){
    
//     filterTypeCode.second=getTypeCode(0);
//   }
// }

// const SHTrigSumMaker::getFilterTypeCode(const std::string& modName,const HLTConfigProvider& config)
// {
//   std::string modType=config.moduleType(modName);
//   if(modType=="HLTLevel1GTSeed") return SHTrigObj::L1S1;
//   else if(modType=="HLTElectronPixelMatchFilter") return SHTrigObj::PHOTON;
//   else if(modType.find("HLTEgamma")==0) return SHTrigObj::PHOTON;
//   else if(modType=="HLTElectronOneOEMinusOneOPFilterRegional") return SHTrigObj::ELECTRON;
//   else if(modType=="HLTElectronGenericFilter") return SHTrigObj::ELECTRON;
//   else {
//     std::cout <<"Unknown filter type "<<modName<<" "<<modType<<std::endl;
//   }
// }


