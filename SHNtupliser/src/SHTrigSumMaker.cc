
#include "SHarper/SHNtupliser/interface/SHTrigSumMaker.h"
#include "SHarper/SHNtupliser/interface/LogErr.hh"
#include "SHarper/SHNtupliser/interface/SHTrigSummary.hh"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"

#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"

int SHTrigSumMaker::verboseLvl_ = 0;

void SHTrigSumMaker::makeSHTrigSum(const heep::Event& heepEvent,SHTrigSummary& shTrigSum)
{
  if(heepEvent.handles().trigEvent.isValid()){
    const trigger::TriggerEvent& trigEvt = heepEvent.triggerEvent();
    
    const edm::TriggerResults& trigResults = *heepEvent.handles().trigResults;
    const edm::TriggerNames& trigNames = heepEvent.event().triggerNames(trigResults);  

    if(!heepEvent.validHLTPSProvider()){
       throw edm::Exception(edm::errors::LogicError,"Error, HLTPrescaleProvider is null, this will end poorly");
    }
    
    makeSHTrigSum(trigEvt,trigResults,trigNames,
		  *heepEvent.hltPSProv(),heepEvent.event(),heepEvent.eventSetup(),
		  shTrigSum);
  }
}


void SHTrigSumMaker::makeSHTrigSum(const trigger::TriggerEvent& trigEvt,
				   const edm::TriggerResults& trigResults,
				   const edm::TriggerNames& trigNames,
				   HLTPrescaleProvider& hltPSProv, 
				   const edm::Event& edmEvent,
				   const edm::EventSetup& edmEventSetup,
				   SHTrigSummary& shTrigSum)
				
{
  const HLTConfigProvider& hltConfig = hltPSProv.hltConfigProvider();  
  if(!hltConfig.inited()){
    throw edm::Exception(edm::errors::LogicError,"Error, HLTConfig is not initialised, this will end poorly");
  }
  

  //there is a small potenial bug here if the menu changes but is kept the same name
  //this can only happen for private production/studies and we will ignore this
  if(hltConfig.tableName()!=shTrigSum.menuName() || true){
    shTrigSum.clearMenu();
    fillMenu_(shTrigSum,hltConfig,hltPSProv.l1GtUtils());
  }
  shTrigSum.setProcessName(hltConfig.processName()); //in theory this could change without chaning menu name and its fine
  shTrigSum.clearEvent();

  //little hack, we're only going to store filters passed, not all filters for space reasons
  std::set<std::string> filterNames;
  for(size_t filterNr=0;filterNr<trigEvt.sizeFilters();filterNr++) filterNames.insert(trigEvt.filterTag(filterNr).label());
  shTrigSum.setFilterBitsDef(filterNames);

  shTrigSum.setPreScaleColumn(hltPSProv.prescaleSet(edmEvent,edmEventSetup));
  fillSHTrigResults_(trigResults,trigNames,hltPSProv,edmEvent,edmEventSetup,shTrigSum);
  fillSHTrigObjs_(trigEvt,shTrigSum);
  fillSHL1Results_(hltPSProv.l1GtUtils(),edmEvent,shTrigSum);
  shTrigSum.sort();
}


void SHTrigSumMaker::fillMenu_(SHTrigSummary& shTrigSum,
			       const HLTConfigProvider& hltConfig,
			       const L1GtUtils& l1GtUtils)
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
      //we need to remove any "-" in front the modulename
      filterNames.insert(savedMod.front()!='-' ? savedMod : savedMod.substr(1));
    }
  }
  
  std::vector<std::string> l1Names;
  
  std::string trigName,trigAlias;
  for(int bitNr=0;bitNr<=127;bitNr++){  //should fix this
    l1GtUtils.l1TriggerNameFromBit(bitNr,L1GtUtils::AlgorithmTrigger,trigAlias,trigName);
      //if(trigAlias!=trigName) std::cout <<"name" <<trigName<<" alias "<<trigAlias<<std::endl;
    l1Names.push_back(trigAlias);
  }
  shTrigSum.setL1Names(l1Names);
  shTrigSum.setPathBitsDef(pathNames);
  shTrigSum.setFilterBitsDef(filterNames);

}

void SHTrigSumMaker::fillSHTrigResults_(const edm::TriggerResults& trigResults,
					const edm::TriggerNames& trigNames,
					HLTPrescaleProvider& hltPSProv,
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
    int prescale=hltPSProv.hltConfigProvider().inited() ? hltPSProv.prescaleValue(edmEvent,edmEventSetup,pathName) : -1;
    auto l1Prescales = getPathL1Prescales_(pathName,hltPSProv,edmEvent,edmEventSetup,shTrigSum);

    SHTrigResult::Status status = static_cast<SHTrigResult::Status>(trigResults[pathNr].state());
    shTrigSum.addTrigResult(SHTrigResult(bitNr,status,prescale,l1Prescales));     
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
    if(bitNr==SHBitsDef::npos) LogErr <<"filter tag "<<trigEvt.filterTag(filterNr).label()<<" not found"<<std::endl;
    for(size_t objNr=0;objNr<trigKeys.size();objNr++){
      auto key = trigKeys[objNr];
      auto id = convertToSHTrigType(trigIds[objNr]);
      shTrigObjsTmp[key].second.first.SetBitNumber(bitNr);
      if(shTrigObjsTmp[key].second.second==0) shTrigObjsTmp[key].second.second=id;
      else if(shTrigObjsTmp[key].second.second!=id && id!=0){
	if(verboseLvl_>0) LogErr<<" Error trigger object type for filter "<<trigEvt.filterTag(filterNr).label()<<" was previously type "<<shTrigObjsTmp[key].second.second<<" but is now type "<<id<<" cmss id "<<trigIds[objNr]<<std::endl;
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

void SHTrigSumMaker::fillSHL1Results_(const L1GtUtils& l1Util,
				      const edm::Event& edmEvent,
				      SHTrigSummary& shTrigSum)
{
  int errorCode;
  for(size_t bitNr=0;bitNr<shTrigSum.l1Names().size();bitNr++){
    const std::string& l1Name = shTrigSum.l1Names()[bitNr];
    bool pass = l1Util.decision(edmEvent,l1Name,errorCode);
    int mask = l1Util.triggerMask(l1Name,errorCode);
    int prescale = l1Util.prescaleFactor(edmEvent,l1Name,errorCode);
    shTrigSum.addL1Result(SHL1Result(bitNr,pass,mask,prescale));
  }
  
}

std::vector<std::pair<size_t,int>> 
SHTrigSumMaker::getPathL1Prescales_(const std::string& pathName,
				    HLTPrescaleProvider& hltPSProv,
				    const edm::Event& edmEvent,
				    const edm::EventSetup& edmEventSetup,
				    SHTrigSummary& shTrigSum)
{
  std::vector<std::pair<size_t,int>> retVal;
  if(!hltPSProv.hltConfigProvider().inited()) return retVal;
  
  auto prescales = hltPSProv.prescaleValuesInDetail(edmEvent,edmEventSetup,pathName);
  const std::vector<std::string>& l1Names = shTrigSum.l1Names();
  for(auto& prescale : prescales.first){
    size_t bitNr=std::distance(l1Names.begin(),std::find(l1Names.begin(),l1Names.end(),prescale.first));
    retVal.push_back(std::pair<size_t,int>(bitNr,prescale.second));
  }
  return retVal;
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
  case trigger::TriggerL1EG: return SHTrigObj::L1EG;
  case trigger::TriggerL1Jet: return SHTrigObj::L1JET;
  case trigger::TriggerL1Tau: return SHTrigObj::L1TAU;
  
  default: return SHTrigObj::UNDEFINED;
  }
}

std::string SHTrigSumMaker::rmTrigVersionFromName(std::string pathName)
{
  size_t versionIndex = pathName.rfind("_v");
  
  if(versionIndex!=std::string::npos) pathName.erase(versionIndex+2);
  return pathName;
}

void SHTrigSumMaker::associateEgHLTDebug(const heep::Event& heepEvent,SHTrigSummary& shTrigSum)
{
  if(heepEvent.handles().egHLTCands.isValid()) associateEgHLTDebug(heepEvent.event(),heepEvent.handles().egHLTCands,shTrigSum);
  if(heepEvent.handles().egHLTCandsUnseeded.isValid()) associateEgHLTDebug(heepEvent.event(),heepEvent.handles().egHLTCandsUnseeded,shTrigSum);
}

void SHTrigSumMaker::associateEgHLTDebug(const edm::Event& edmEvent,const edm::Handle<std::vector<reco::RecoEcalCandidate>>& ecalCands,SHTrigSummary& shTrigSum)
{
  for(auto& ecalCand : *ecalCands){
    reco::RecoEcalCandidateRef ecalCandRef(ecalCands,&ecalCand -&(*ecalCands)[0]);
    std::vector<SHTrigObj*> matchedObjs = shTrigSum.getTrigObjs(ecalCand.eta(),ecalCand.phi(),SHTrigObj::HLT,0.001);
    if(!matchedObjs.empty()) associateEgHLTDebug(edmEvent,ecalCandRef,matchedObjs);
  }
}

void SHTrigSumMaker::associateEgHLTDebug(const edm::Event& edmEvent,const reco::RecoEcalCandidateRef& ecalCand,const std::vector<SHTrigObj*> trigObjs)
{
  //first we need to retrieve all the value maps and get their names in a nice little format
  std::vector<edm::Handle<reco::RecoEcalCandidateIsolationMap> > valueMapHandles;
  edmEvent.getManyByType(valueMapHandles);
  std::vector<std::pair<std::string,float>> values;
  for(auto& valueMapHandle : valueMapHandles){
    auto mapIt = valueMapHandle->find(ecalCand);
    if(mapIt!=valueMapHandle->end()){
      std::string name=valueMapHandle.provenance()->moduleLabel()+valueMapHandle.provenance()->productInstanceName();
      values.push_back(std::pair<std::string,float>(name,mapIt->val));
    }
  }
  
  for(auto trigObj : trigObjs){
    trigObj->addVars(values);
  }
  
  
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


