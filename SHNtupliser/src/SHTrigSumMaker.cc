
#include "SHarper/SHNtupliser/interface/SHTrigSumMaker.h"
#include "SHarper/SHNtupliser/interface/LogErr.hh"
#include "SHarper/SHNtupliser/interface/SHTrigSummary.hh"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"

#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"


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
  //we are now forcing a change if the process name changes as well
  if(hltConfig.tableName()!=shTrigSum.menuName() || hltConfig.processName()!=shTrigSum.processName()){
    std::cout <<"making a new menu"<<std::endl;
    updateCacheMenuChange_(edmEvent,edmEventSetup,hltPSProv);
  }else if(!lastRunLumi_.sameRun({edmEvent.id().run(),edmEvent.luminosityBlock()}) ){
    lastRunLumi_={edmEvent.id().run(),edmEvent.luminosityBlock()};
    updateCacheRunChange_(edmEvent,edmEventSetup,hltPSProv);
  }else if(!lastRunLumi_.sameLumi({edmEvent.id().run(),edmEvent.luminosityBlock()}) ){
    lastRunLumi_={edmEvent.id().run(),edmEvent.luminosityBlock()};
    updateCacheLumiChange_(edmEvent,edmEventSetup,hltPSProv);
  }

  shTrigSum.clearEvent();

  shTrigSum.setPreScaleColumn(currPSColumn_); 

  //fix for issue where methods in L1TGlobalUtil were not properly declared const
  fillMenu_(shTrigSum,hltConfig,const_cast<l1t::L1TGlobalUtil&>(hltPSProv.l1tGlobalUtil()));
  fillSHTrigResults_(trigResults,trigNames,hltPSProv,shTrigSum); 
  fillSHTrigObjs_(trigEvt,shTrigSum);
  fillSHL1Results_(const_cast<l1t::L1TGlobalUtil&>(hltPSProv.l1tGlobalUtil()),edmEvent,shTrigSum);

  shTrigSum.sort();
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

void SHTrigSumMaker::fillMenu_(SHTrigSummary& shTrigSum,
			       const HLTConfigProvider& hltConfig,
			       l1t::L1TGlobalUtil& l1GtUtils)
{
  const SHTrigMenuData& menuData = getMenuData_(hltConfig,l1GtUtils);
  shTrigSum.setL1Names(menuData.l1Names());
  shTrigSum.setPathBitsDef(menuData.pathBitsDef());
  shTrigSum.setFilterBitsDef(menuData.filterBitsDef());
  shTrigSum.setMenuName(menuData.menuName());
  shTrigSum.setProcessName(menuData.processName());
  shTrigSum.setGlobalTag(menuData.globalTag());  
}

const SHTrigMenuData&
SHTrigSumMaker::getMenuData_(const HLTConfigProvider& hltConfig,
				  l1t::L1TGlobalUtil& l1GtUtils)
{
  if(!menuMgr_.hasMenu(hltConfig.tableName(),hltConfig.processName())){
    addMenuData_(hltConfig,l1GtUtils);
  }
  return menuMgr_.get(hltConfig.tableName(),hltConfig.processName());
}

void SHTrigSumMaker::addMenuData_(const HLTConfigProvider& hltConfig,
				  l1t::L1TGlobalUtil& l1GtUtils)
{
  
  SHTrigMenuData menuData;
  menuData.setProcessName(hltConfig.processName());
  menuData.setGlobalTag(hltConfig.globalTag());
  menuData.setMenuName(hltConfig.tableName());
  
  std::vector<std::string> pathNames;
  for(auto& pathName : hltConfig.triggerNames()){
    pathNames.emplace_back(rmTrigVersionFromName(pathName));
  }

  std::vector<std::string> filterNames;    
  for(size_t trigNr=0;trigNr<hltConfig.size();trigNr++){
    const std::vector<std::string>& savedMods = hltConfig.saveTagsModules(trigNr);
    for(auto& savedMod : savedMods){
      //we need to remove any "-" in front the modulename
      //emplacing for the tiny performance boost when there is "-" in the front of the name
      filterNames.emplace_back(savedMod.front()!='-' ? savedMod : savedMod.substr(1));
    }
  }
  
  std::vector<std::string> l1Names(l1GtUtils.prescales().size());
  std::transform(l1GtUtils.prescales().begin(),l1GtUtils.prescales().end(),l1Names.begin(),
		 [](const std::pair<std::string,int>& val){return val.first;});
  
  menuData.setL1Names(l1Names);
  menuData.setPathBitsDef(pathNames);
  menuData.setFilterBitsDef(filterNames);  
  
  menuMgr_.add(menuData);
}

void SHTrigSumMaker::fillSHTrigResults_(const edm::TriggerResults& trigResults,
					const edm::TriggerNames& trigNames,
					HLTPrescaleProvider& hltPSProv,
					SHTrigSummary& shTrigSum)const
					
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
    int prescale=hltPSProv.hltConfigProvider().inited() ? hltPSProv.hltConfigProvider().prescaleValue(currPSColumn_,pathName) : -1;
    auto l1Prescales = getPathL1Prescales_(pathName);

    SHTrigResult::Status status = static_cast<SHTrigResult::Status>(trigResults[pathNr].state());

    if(pathNameWOVersion=="HLT_Photon36wdwq_v"){
      std::cout <<"for  "<<pathNameWOVersion<<std::endl;
      for(auto ps : l1Prescales){
	std::cout <<"   l1 PS "<<ps.first<<" "<<ps.second<<" l1Prescale "<<std::endl;
      }
    }

    shTrigSum.addTrigResult(SHTrigResult(bitNr,status,prescale,l1Prescales));     
  } 
}



void SHTrigSumMaker::fillSHTrigObjs_(const trigger::TriggerEvent& trigEvt,
				     SHTrigSummary& shTrigSum)const
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

    //might be better to use filterLabel()
    size_t bitNr=shTrigSum.filterBitsDef().getBitNr(trigEvt.filterLabel(filterNr));
    if(bitNr==SHBitsDef::npos) LogErr <<"filter tag "<<trigEvt.filterLabel(filterNr)<<" not found"<<std::endl;

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

void SHTrigSumMaker::fillSHL1Results_(l1t::L1TGlobalUtil& l1Util,
				      const edm::Event& edmEvent,
				      SHTrigSummary& shTrigSum)
{
  // std::cout <<"starting ps dump "<<l1Util.prescales().size()<<std::endl;
  // for(auto& ps : l1Util.prescales()){
  //   std::cout <<ps.first<<" "<<ps.second<<std::endl;
  // }

  for(size_t bitNr=0;bitNr<shTrigSum.l1Names().size();bitNr++){
    bool pass = l1Util.decisionsFinal()[bitNr].second;
    int mask = l1Util.masks()[bitNr].second;
    int prescale = l1Util.prescales()[bitNr].second;
    shTrigSum.addL1Result(SHL1Result(bitNr,pass,mask,prescale));

    if(shTrigSum.l1Names()[bitNr]!=l1Util.decisionsFinal()[bitNr].first ||
       shTrigSum.l1Names()[bitNr]!=l1Util.masks()[bitNr].first ||
       shTrigSum.l1Names()[bitNr]!=l1Util.prescales()[bitNr].first){
      LogErr<<" Warning, algo names dont match up "<<shTrigSum.l1Names()[bitNr]<<" "<<l1Util.decisionsFinal()[bitNr].first <<" "<<l1Util.masks()[bitNr].first<<" "<<l1Util.prescales()[bitNr].first<<std::endl;
    }
    

  }
  
}


std::vector<std::pair<size_t,int> > 
SHTrigSumMaker::getPathL1Prescales_(const std::string& pathName)const
{
  std::vector<std::pair<size_t,int> > retVal;
  
  auto pathL1Seeds =  std::find(pathsL1Seeds_.begin(),
				pathsL1Seeds_.end(),pathName); 

  if(pathL1Seeds!=pathsL1Seeds_.end()){
    for(size_t bitNr : pathL1Seeds->seeds()){
      int prescale=-1;
      if(bitNr<l1Prescales_.size()) prescale=l1Prescales_[bitNr];
      else LogErr<<" L1 bit "<<bitNr<<" out of range (max = "<<l1Prescales_.size()<<" )"<<std::endl;
      retVal.push_back(std::pair<size_t,int>(bitNr,prescale));
    }
  }
  return retVal;
}


std::vector<size_t> SHTrigSumMaker::getL1Seeds_(const std::string& pathName,
						const std::vector<std::string>& l1Names,
						const HLTConfigProvider& hltConfig)const
{
  std::vector<size_t> retVal;
  if(!hltConfig.inited()) return retVal;
  
  //  std::cout <<"getting L1 seeds"<<std::endl;
  auto l1SeedMods = hltConfig.hltL1TSeeds(pathName);
  //  std::cout <<"got l1 seed mods"<<std::endl;
  if(l1SeedMods.size()!=1) {
    if(!l1SeedMods.empty() && pathName.find("HLT_Mu3_PFJet40_v")==std::string::npos) LogErr<<pathName<<" has more than 1 L1GTSeed "<<l1SeedMods.size()<<std::endl;
    return retVal;
  }
  //std::cout <<"seed mode "<<l1SeedMods[0]<<std::endl;
  auto seedNames = splitL1SeedExpr_(l1SeedMods[0]);
  if(seedNames.size()>kMaxNrL1SeedsToStore_) return retVal;
  
  for(auto& l1SeedName : seedNames){
    size_t bitNr=std::distance(l1Names.begin(),std::find(l1Names.begin(),l1Names.end(),l1SeedName));
    //   std::cout <<"name "<<l1SeedName<<" "<<bitNr<<std::endl;
    retVal.push_back(bitNr);
  }
  return retVal;
  
}
				 
				 
void SHTrigSumMaker::updateCacheMenuChange_(const edm::Event& edmEvent, 
					    const edm::EventSetup& edmEventSetup, 
					    HLTPrescaleProvider& hltPSProv)
{
  //std::cout <<"duming paths"<<std::endl;
  auto& hltConfig  = hltPSProv.hltConfigProvider();
  
  pathsL1Seeds_.clear();
  pathsL1Seeds_.reserve(hltConfig.triggerNames().size());
  const std::vector<std::string>& l1Names = menuMgr_.get(hltConfig.tableName(),hltConfig.processName()).l1Names();
  
  for(auto& pathName : hltConfig.triggerNames()  ){
    //std::cout <<"path "<<pathName<<std::endl;
    pathsL1Seeds_.emplace_back(PathL1Seeds(rmTrigVersionFromName(pathName),
					   getL1Seeds_(pathName,l1Names,hltConfig)
					   )); //name + seeds;
  }
  updateCacheRunChange_(edmEvent,edmEventSetup,hltPSProv);

}

void SHTrigSumMaker::updateCacheRunChange_(const edm::Event& edmEvent,
					   const edm::EventSetup& edmEventSetup, 
					   HLTPrescaleProvider& hltPSProv)
{
  //l1PreScaleTbl currently out of action (grr l1)
  l1PreScaleTbl_.clear();
  //  l1PreScaleTbl_.reserve(l1Names);
  // for(auto& l1Name : l1Names){
 
  updateCacheLumiChange_(edmEvent,edmEventSetup,hltPSProv);
  
}


void SHTrigSumMaker::updateCacheLumiChange_(const edm::Event& edmEvent,
					    const edm::EventSetup& edmEventSetup, 
					    HLTPrescaleProvider& hltPSProv)
{
  currPSColumn_= hltPSProv.prescaleSet(edmEvent,edmEventSetup);
  auto& l1Util = const_cast<l1t::L1TGlobalUtil&>(hltPSProv.l1tGlobalUtil()); //because of silly L1 const issues
  
  l1Prescales_.clear();
  l1Prescales_.reserve(l1Util.prescales().size());
  for(auto& ps : l1Util.prescales()){
    l1Prescales_.push_back(ps.second);
  }
}

#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
std::vector<std::string> SHTrigSumMaker::splitL1SeedExpr_(const std::string& l1SeedExpr)
{
  std::vector<std::string> seedNames;
  boost::algorithm::split_regex( seedNames, l1SeedExpr, boost::regex( " OR " ) ) ;
  for(auto& seedName : seedNames) boost::algorithm::trim( seedName);
  return seedNames; 
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


