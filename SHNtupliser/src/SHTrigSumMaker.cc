
#include "SHarper/SHNtupliser/interface/SHTrigSumMaker.h"
#include "SHarper/SHNtupliser/interface/LogErr.hh"
#include "SHarper/SHNtupliser/interface/SHTrigSummary.hh"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"

#include "CondFormats/DataRecord/interface/L1TUtmTriggerMenuRcd.h"
#include "CondFormats/L1TObjects/interface/L1TUtmTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1TGlobalPrescalesVetosRcd.h"
#include "CondFormats/L1TObjects/interface/L1TGlobalPrescalesVetos.h"

#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"


void SHTrigSumMaker::makeSHTrigSum(const heep::Event& heepEvent,SHTrigSummary& shTrigSum)
{
  if(heepEvent.handles().trigResults.isValid()){

    const edm::TriggerResults& trigResults = *heepEvent.handles().trigResults;
    const edm::TriggerNames& trigNames = heepEvent.event().triggerNames(trigResults);  

    if(!heepEvent.validHLTPSProvider()){
       throw edm::Exception(edm::errors::LogicError,"Error, HLTPrescaleProvider is null, this will end poorly");
    }
    
    makeSHTrigSumNoFilters_(trigResults,trigNames,*heepEvent.hltPSProv(),heepEvent.event(),heepEvent.eventSetup(),
			    shTrigSum);
    
    if(heepEvent.handles().trigEvent.isValid()){
      fillSHTrigObjs_(heepEvent.triggerEvent(),shTrigSum);
    }else if(heepEvent.handles().patTrigObjs.isValid()){
      std::vector<pat::TriggerObjectStandAlone> unpackedObjs;
      for(auto& trigObj : *heepEvent.handles().patTrigObjs){
	unpackedObjs.push_back(trigObj);
	unpackedObjs.back().unpackFilterLabels(heepEvent.event(),trigResults);
      }
      fillSHTrigObjs_(unpackedObjs,shTrigSum);
    }
    shTrigSum.sort();
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
  makeSHTrigSumNoFilters_(trigResults,trigNames,hltPSProv,edmEvent,edmEventSetup,shTrigSum);
  fillSHTrigObjs_(trigEvt,shTrigSum);
  shTrigSum.sort();
  
}

void SHTrigSumMaker::makeSHTrigSum(std::vector<pat::TriggerObjectStandAlone>& trigObjs,
				   const edm::TriggerResults& trigResults,
				   const edm::TriggerNames& trigNames,
				   HLTPrescaleProvider& hltPSProv, 
				   const edm::Event& edmEvent,
				   const edm::EventSetup& edmEventSetup,
				   SHTrigSummary& shTrigSum)
				
{
  makeSHTrigSumNoFilters_(trigResults,trigNames,hltPSProv,edmEvent,edmEventSetup,shTrigSum);
  fillSHTrigObjs_(trigObjs,shTrigSum);
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

int SHTrigSumMaker::convertToSHTrigType(const pat::TriggerObject& trigObj)
{
  int trigType=0x0;
  for(int cmsswType : trigObj.triggerObjectTypes()){
    trigType|=convertToSHTrigType(cmsswType);
  }
  if(isUnseededEG(trigObj.collection()))  trigType|=SHTrigObj::EGUNSEEDED;
     // if(trigObj.hasCollection("hltEgammaCandidatesUnseeded")) trigType&=SHTrigObj::EGUNSEEDED;
  //  std::cout <<trigObj.collection()<<" "<<trigObj.hasCollection("hltEgammaCandidatesUnseeded")<<" "<<trigObj.collection().compare(0,27,"hltEgammaCandidatesUnseeded")<<std::endl;
  return trigType;
}

std::string SHTrigSumMaker::rmTrigVersionFromName(std::string pathName)
{
  size_t versionIndex = pathName.rfind("_v");
  
  if(versionIndex!=std::string::npos) pathName.erase(versionIndex+2);
  return pathName;
}

bool SHTrigSumMaker::isUnseededEG(const trigger::TriggerEvent& trigEvt,size_t trigObjNr)
{
  const trigger::Keys& collKeys = trigEvt.collectionKeys();
  size_t collNr=0;
  while(trigObjNr>=collKeys[collNr]) collNr++; 
  auto& collName = trigEvt.collectionTagEncoded(collNr);
  return isUnseededEG(collName);
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


void SHTrigSumMaker::makeSHTrigSumNoFilters_(const edm::TriggerResults& trigResults,
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

  //tempory L1 menu to work around L1 software issues, we have to invent a name
  std::string l1MenuName("l1MenuForRun"+std::to_string(edmEvent.id().run()));
  if(!l1Menu_ || l1Menu_->menuName()!=l1MenuName){
    std::cout <<"making a new l1 menu "<<l1MenuName<<std::endl;
    //    l1Menu_ = &getL1Menu_(hltConfig,hltPSProv.l1tGlobalUtil(),l1MenuName);
    l1Menu_ = &getL1Menu_(edmEventSetup,l1MenuName);
  }
  //there is a small potenial bug here if the menu changes but is kept the same name
  //this can only happen for private production/studies and we will ignore this
  //we are now forcing a change if the process name changes as well
  if(!hltMenu_ || 
     hltConfig.tableName()!=hltMenu_->menuName() || 
     hltConfig.processName()!=hltMenu_->processName()){
    std::cout <<"making a new hlt menu "<<hltConfig.tableName()<<std::endl;
    hltMenu_ = &getHLTMenu_(hltConfig);
  }
 
  if(!lastRunLumi_.sameRun({edmEvent.id().run(),edmEvent.luminosityBlock()}) ){
    lastRunLumi_={edmEvent.id().run(),edmEvent.luminosityBlock()};
    updateCacheRunChange_(edmEvent,edmEventSetup,hltPSProv);
  }else if(!lastRunLumi_.sameLumi({edmEvent.id().run(),edmEvent.luminosityBlock()}) ){
    lastRunLumi_={edmEvent.id().run(),edmEvent.luminosityBlock()};
    updateCacheLumiChange_(edmEvent,edmEventSetup,hltPSProv);
  }

  shTrigSum.clearEvent();
  shTrigSum.setPreScaleColumn(currPSColumn_); 
  fillMenu_(shTrigSum);
  shTrigSum.setL1Result(getL1Result_(hltPSProv.l1tGlobalUtil()));
  shTrigSum.setHLTResult(getHLTResult_(trigResults));
}


void SHTrigSumMaker::fillMenu_(SHTrigSummary& shTrigSum)const			       
{
  shTrigSum.setL1Menu(l1Menu_);
  shTrigSum.setHLTMenu(hltMenu_);
  shTrigSum.setL1MenuName(l1Menu_->menuName());
  shTrigSum.setMenuName(hltMenu_->menuName());
  shTrigSum.setProcessName(hltMenu_->processName());  
}

const SHL1Menu&
SHTrigSumMaker::getL1Menu_(const HLTConfigProvider& hltConfig,
			   const l1t::L1TGlobalUtil& l1GtUtils,
			   const std::string& l1MenuName)
{
  if(!menuMgr_.hasL1Menu(l1MenuName)){
    addL1Menu_(hltConfig,l1GtUtils,l1MenuName);
  }
  return menuMgr_.getL1(l1MenuName);
}

const SHL1Menu&
SHTrigSumMaker::getL1Menu_(const edm::EventSetup& edmEventSetup,
			   const std::string& l1MenuName)
{
  if(!menuMgr_.hasL1Menu(l1MenuName)){
    addL1Menu_(edmEventSetup,l1MenuName);
  }
  return menuMgr_.getL1(l1MenuName);
}

const SHHLTMenu&
SHTrigSumMaker::getHLTMenu_(const HLTConfigProvider& hltConfig)

{
  if(!menuMgr_.hasHLTMenu(hltConfig.tableName(),hltConfig.processName())){
    addHLTMenu_(hltConfig);
  }
  return menuMgr_.getHLT(hltConfig.tableName(),hltConfig.processName());
}

void SHTrigSumMaker::addHLTMenu_(const HLTConfigProvider& hltConfig)
			
{  
  std::vector<SHHLTMenu::Path> hltPaths;
  std::vector<unsigned int> defaultPSes(hltConfig.prescaleSize(),1);
  for(size_t pathNr=0;pathNr<hltConfig.triggerNames().size();pathNr++){
    const std::string& trigName  = hltConfig.triggerNames()[pathNr];

    std::vector<std::string> l1Seeds = getL1Seeds_(hltConfig,pathNr,trigName);
    if(l1Seeds.size()>kMaxNrL1SeedsToStore_) l1Seeds.clear();
    
    auto psTblEntry = hltConfig.prescaleTable().find(trigName);
    const std::vector<unsigned int>& preScales = psTblEntry!=hltConfig.prescaleTable().end() ? 
      psTblEntry->second : defaultPSes;
    
    hltPaths.emplace_back(SHHLTMenu::Path(pathNr,rmTrigVersionFromName(trigName),
					  preScales,std::move(l1Seeds)));
    
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
  
  SHHLTMenu hltMenu;
  hltMenu.setProcessName(hltConfig.processName());
  hltMenu.setGlobalTag(hltConfig.globalTag());
  hltMenu.setMenuName(hltConfig.tableName());
  hltMenu.setFilterBitsDef(filterNames);   
  hltMenu.setPaths(std::move(hltPaths));
  menuMgr_.add(hltMenu);
}

void SHTrigSumMaker::addL1Menu_(const HLTConfigProvider& hltConfig,
				const l1t::L1TGlobalUtil& l1GtUtilsConst,
				const std::string& l1MenuName)
{
  std::vector<SHL1Menu::Seed> seeds;

  //because L1TGlobalUtil is garbage and broken
  l1t::L1TGlobalUtil& l1GtUtils = const_cast<l1t::L1TGlobalUtil&>(l1GtUtilsConst);

  std::vector<unsigned int> defaultPSes(hltConfig.prescaleSize(),1);
  for(size_t bitNr=0;bitNr<l1GtUtils.decisionsFinal().size();bitNr++){
    const std::string& l1Name = l1GtUtils.decisionsFinal()[bitNr].first;
    seeds.emplace_back(SHL1Menu::Seed(bitNr,l1Name,defaultPSes,false));
  }
  SHL1Menu l1Menu;
  l1Menu.setMenuName(l1MenuName);
  l1Menu.setSeeds(std::move(seeds));

  menuMgr_.add(l1Menu);
  // like all L1 software, this is broken
  //  std::cout <<"l1 "<<l1GtUtils.gtTriggerMenuName()<<" "<<
  // l1GtUtils.gtTriggerMenuVersion()<<
  // l1GtUtils.gtTriggerMenuComment()<<std::endl;
 		  
 
}

void SHTrigSumMaker::addL1Menu_(const edm::EventSetup& edmEventSetup,
				const std::string& l1MenuName)
{
  std::vector<SHL1Menu::Seed> seeds;

  
  edm::ESHandle<L1TUtmTriggerMenu> l1GtMenu;
  edmEventSetup.get<L1TUtmTriggerMenuRcd>().get(l1GtMenu);
  
  edm::ESHandle<L1TGlobalPrescalesVetos> psAndVetos;
  auto psRcd = edmEventSetup.tryToGet<L1TGlobalPrescalesVetosRcd>();
  if(psRcd) psRcd->get(psAndVetos);
  
  std::array<std::string,kNrL1Seeds_> l1Names;
  l1Names.fill("NULL");
  for(auto& entry : l1GtMenu->getAlgorithmMap()){
    if(entry.first!=entry.second.getName()){
      LogErr<<l1MenuName<<" inconsistant l1AlgoMap, "<<entry.first<<" should equal "<<entry.second.getName()<<std::endl;
      throw cms::Exception("LogicError");
    }
    if(entry.second.getIndex()<l1Names.size()){
      l1Names[entry.second.getIndex()]=entry.second.getName();
    }else{
      LogErr<<l1MenuName<<" inconsistant l1AlgoMap, "<<entry.first<<" with index "<<entry.second.getIndex()<<" is out of range "<<l1Names.size()<<std::endl;
      throw cms::Exception("LogicError");
    }
  }
  
  if(psAndVetos.isValid()){
    if(psAndVetos->veto_.size()!=kNrL1Seeds_){
      LogErr<<l1MenuName<<" vetos has "<<psAndVetos->veto_.size()<<" not "<<kNrL1Seeds_<<std::endl;
    }

    for(size_t bitNr=0;bitNr<l1Names.size();bitNr++){
      auto preScales = getSeedPreScales(bitNr,psAndVetos->prescale_table_);
      bool veto = bitNr<psAndVetos->veto_.size() ? psAndVetos->veto_[bitNr] : true;
      seeds.emplace_back(SHL1Menu::Seed(bitNr,l1Names[bitNr],std::move(preScales),veto));
    }
  }else{
    for(size_t bitNr=0;bitNr<l1Names.size();bitNr++){
      seeds.emplace_back(SHL1Menu::Seed(bitNr,l1Names[bitNr],std::vector<unsigned int>(),false));
    }
  }

  SHL1Menu l1Menu;
  l1Menu.setMenuName(l1MenuName);
  l1Menu.setSeeds(std::move(seeds));

  menuMgr_.add(l1Menu);
 
}


TBits SHTrigSumMaker::getL1Result_(const l1t::L1TGlobalUtil& l1UtilsConst)const
{
  // std::cout <<"starting ps dump "<<l1Util.prescales().size()<<std::endl;
  // for(auto& ps : l1Util.prescales()){
  //   std::cout <<ps.first<<" "<<ps.second<<std::endl;
  // }
  //because L1TGlobalUtil is garbage and broken
  l1t::L1TGlobalUtil& l1Util = const_cast<l1t::L1TGlobalUtil&>(l1UtilsConst);

  if(!l1Menu_){
    LogErr<<" Error, L1 menu not set"<<std::endl;
    throw cms::Exception("BadSHL1Menu");
  }
  // if(l1Menu_->l1Seeds().size()!=l1Util.decisionsFinal().size()){
  //   //    LogErr<<"  Error SHL1Menu has "<<l1Menu_->l1Seeds().size()<<" while CMSSW L1 menu has "<<l1Util.decisionsFinal().size()<<" seeds"<<std::endl;
  //   throw cms::Exception("BadSHL1Menu");
  // }

  TBits result(l1Util.decisionsFinal().size());
  for(size_t bitNr=0;bitNr<l1Util.decisionsFinal().size();bitNr++){
    bool pass = l1Util.decisionsFinal()[bitNr].second;
    if(pass) result.SetBitNumber(bitNr);
 
    //    if(l1Menu_->getSeed(bitNr).name()!="NULL") std::cout <<"bit nr "<<bitNr<<" "<<l1Menu_->getSeed(bitNr).name()<<" prescale "<<l1Util.prescales()[bitNr].first<<" "<<l1Util.prescales()[bitNr].second<<" mine "<<l1Menu_->getSeed(bitNr).preScale(currPSColumn_)<<std::endl;

    const std::string& l1Name = l1Menu_->getSeed(bitNr).name();
    if(l1Name!=l1Util.decisionsFinal()[bitNr].first){
          // l1Name!=l1Util.masks()[bitNr].first ||  //masks are screwed
       // l1Name!=l1Util.prescales()[bitNr].first){
      LogErr<<" Warning, algo names dont match up for bit "<<bitNr<<" SHL1MenuName "<<l1Name<<" L1DecisionName "<<l1Util.decisionsFinal()[bitNr].first <<" L1MaskName "<<l1Util.masks()[bitNr].first<<" L1PSName "<<l1Util.prescales()[bitNr].first<<std::endl;
    }
    
  }
  return result;
}
TBits SHTrigSumMaker::getHLTResult_(const edm::TriggerResults& trigResults)const
{
  TBits result(trigResults.size());
  for(size_t pathNr=0;pathNr<trigResults.size();pathNr++){
    if(trigResults[pathNr].accept()) result.SetBitNumber(pathNr);
  }
  return result;
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
  for(auto& trigObj : trigObjs){
    size_t trigObjNr = shTrigObjsTmp.size();
    int type = isUnseededEG(trigEvt,trigObjNr) ? SHTrigObj::EGUNSEEDED : 0;
    shTrigObjsTmp.push_back({{trigObj.pt(),trigObj.eta(),trigObj.phi(),trigObj.mass()},
	  {TBits(),type}});
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
      shTrigObjsTmp[key].second.second |=id;
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


void SHTrigSumMaker::fillSHTrigObjs_(const std::vector<pat::TriggerObjectStandAlone>& trigObjs,
				     SHTrigSummary& shTrigSum)const
{
  for(auto& trigObj : trigObjs){
    //    std::cout <<trigObj.collection()<<std::endl;
    TBits bits = shTrigSum.filterBitsDef().getBits(trigObj.filterLabels());
    int trigType = convertToSHTrigType(trigObj);
    shTrigSum.addTrigObj(SHTrigObj(trigObj.pt(),trigObj.eta(),trigObj.phi(),
				   trigObj.mass(),trigType,bits));
  }
}

std::vector<std::string> 
SHTrigSumMaker::getL1Seeds_(const HLTConfigProvider& hltConfig,
			    const size_t pathNr,
			    const std::string& pathName)const
{
  if(!hltConfig.inited() || pathNr>=hltConfig.hltL1TSeeds().size()) return std::vector<std::string>();
  
  //  std::cout <<"getting L1 seeds"<<std::endl;
  auto l1SeedMods = hltConfig.hltL1TSeeds(pathNr);
  //  std::cout <<"got l1 seed mods"<<std::endl;
  if(l1SeedMods.size()!=1) {
    if(!l1SeedMods.empty() && pathName.find("HLT_Mu3_PFJet40_v")==std::string::npos) LogErr<<pathName<<" has more than 1 L1GTSeed "<<l1SeedMods.size()<<std::endl;
    return std::vector<std::string>();
  }
  //std::cout <<"seed mode "<<l1SeedMods[0]<<std::endl;
  return splitL1SeedExpr_(l1SeedMods[0]);
}
 
  
				 
			       
void SHTrigSumMaker::updateCacheRunChange_(const edm::Event& edmEvent,
					   const edm::EventSetup& edmEventSetup, 
					   HLTPrescaleProvider& hltPSProv)
{
  
  updateCacheLumiChange_(edmEvent,edmEventSetup,hltPSProv);
  
}


void SHTrigSumMaker::updateCacheLumiChange_(const edm::Event& edmEvent,
					    const edm::EventSetup& edmEventSetup, 
					    HLTPrescaleProvider& hltPSProv)
{
  currPSColumn_= hltPSProv.prescaleSet(edmEvent,edmEventSetup);
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


//converts vector[columnNr][bitNr] -> vector[columnNr] for a given bitNr 
std::vector<unsigned int> 
SHTrigSumMaker::getSeedPreScales(size_t bitNr,const std::vector<std::vector<int> >& psTbl)
{
  if(psTbl.empty()) return std::vector<unsigned int>();

  bool badTbl=false;
  for(size_t colNr=1;colNr<psTbl.size() && !badTbl;colNr++){
    if(psTbl[0].size()!=psTbl[colNr].size()) badTbl=true;    
  }
  
  if(badTbl){
    LogErr<<" Error, prescale columns have different number of bits:";
    for(size_t colNr=0;colNr<psTbl.size();colNr++) std::cout <<" "<<colNr<<":"<<psTbl[colNr].size();
    std::cout <<std::endl;
  }

  if(badTbl || bitNr>=psTbl[0].size()) return std::vector<unsigned int>(psTbl.size(),1);
  else {
    std::vector<unsigned int> preScales;
    for(size_t colNr=0;colNr<psTbl.size();colNr++){
      preScales.push_back(psTbl[colNr][bitNr]);
    }
    return preScales;
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


