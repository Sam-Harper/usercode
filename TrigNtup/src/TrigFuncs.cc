#include "SHarper/TrigNtup/interface/TrigFuncs.hh"

#include "SHarper/SHNtupliser/interface/AnaFuncs.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"
#include "SHarper/TrigNtup/interface/TrigBitsDef.hh"
#include "SHarper/SHNtupliser/interface/SHTrigObjContainer.hh"
#include "SHarper/SHNtupliser/interface/SHTrigSummary.hh" 

const std::vector<std::string> TrigFuncs::l1FilterNames_={"hltL1sSingleJet170or180or200","hltL1sSingleJet170IorSingleJet180IorSingleJet200","hltL1sSingleEG40IorSingleJet200","hltL1sSingleEG34IorSingleEG40","hltL1sSingleEG40IorSingleEG30IorSingleEG26IorDoubleEG2210IorDoubleEG2417IorSingleJet200","hltL1sSingleEG40IorSingleEG30IorSingleEG26IorDoubleEG2210IorDoubleEG2417IorL1EGHighPtSeeds","hltL1sSingleAndDoubleEGNonIsoOrWithEG26WithJetAndTau"};
const std::vector<std::pair<int,std::vector<int> > > TrigFuncs::l1EG26Prescales_=createEG26PSLookup();
TrigFuncs::PreScaleWeights::PreScaleWeights():maxDR2_(0.1*0.1)
{
  std::vector<std::pair<int,int> > etPairs={{30,36},{36,50},{50,75},{75,90},{90,120},{120,175},{175,99999999}};
  for(const auto etPair : etPairs){
    std::ostringstream etStr;
    etStr<<etPair.first;
    bool l1EG26=false;
    if(etPair.first==30 || etPair.first==36) l1EG26=true;
    pathNames_.push_back(PathAndFilters("HLT_Photon"+etStr.str()+"_v","hltEG"+etStr.str()+"HEFilter",
					etPair.first,etPair.second,
					l1EG26));
  }
  filterNames_.resize(pathNames_.size());
  std::transform(pathNames_.begin(),pathNames_.end(),filterNames_.begin(),
		 [](const PathAndFilters& path){return path.filterName;});
}

int TrigFuncs::PreScaleWeights::getWeight(const SHElectron* ele,const SHEvent* event,TLorentzVector& matchedTrigP4)
{
  matchedTrigP4.SetPtEtaPhiM(0,0,0,0);
  if(currEvent_!=*event){
    currEvent_=*event;
    setTrigMatches(event);
  }
  
  const SHTrigSummary& trigSum = event->getTrigSum(); 
  const SHTrigObj* trigObj=nullptr;
  for(auto& match : eleTrigMatches_){
    if(std::get<0>(match)==ele){
      trigObj=std::get<1>(match);
      matchedTrigP4=trigObj->p4();
      break;
    }
  }
  
  for(auto& pathData : pathNames_){
    const SHTrigResult& trigResult = trigSum.getTrig(pathData.pathName);
    if(trigResult.valid()){
      
      if(trigResult.accept() && trigObj && trigObj->valid()) {
	// if(trigObj->pt()>30 && trigObj->pt()<50){
	//   std::cout <<" pt "<<trigObj->pt()<<" "<<std::endl;
	//   auto l1PS = trigResult.l1PreScales();
	//   for(auto& ps : l1PS) std::cout <<"     l1 ps "<<ps.first<<" "<<ps.second<<std::endl;
	// }
	if(trigObj->pt()>=pathData.minEt && trigObj->pt()<pathData.maxEt){
	  int l1PS = 1;	 
	  if(pathData.l1EG26) l1PS=eg26PS(event->runnr(),trigSum.preScaleColumn());
	  
	  return trigResult.preScale()*l1PS;
	}
      }
    }
  }
  return 0;
}

void TrigFuncs::PreScaleWeights::setTrigMatches(const SHEvent* event)
{
  eleTrigMatches_.clear();
  auto trigObjs = event->getTrigSum().getTrigObjs(filterNames_);
  for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
    const SHElectron* ele = event->getElectron(eleNr);
    for(auto trigObj : trigObjs){
      eleTrigMatches_.push_back(std::make_tuple(ele,trigObj,trigObj->deltaR2(ele->detEta(),ele->detPhi())));
    }
  }
  std::sort(eleTrigMatches_.begin(),eleTrigMatches_.end(),
	    [](const EleTrigMatch& lhs,const EleTrigMatch& rhs){return std::get<2>(lhs) < std::get<2>(rhs);});
  bool debug = false;
  if(debug){
    std::cout <<"before cleaning "<<std::endl;
    for(auto & match : eleTrigMatches_){
      auto ele = std::get<0>(match);
      auto trig = std::get<1>(match);
      auto dR = std::get<2>(match);
      std::cout <<ele->et()<<" "<<ele->detEta()<<" "<<ele->detPhi()<<" : trig "<<trig->pt()<<" "<<trig->eta()<<" "<<trig->phi()<<" : dR "<<dR<<std::endl;
    }
  }

  std::set<const SHElectron*> eleMatched;
  std::set<const SHTrigObj*> trigMatched;
  auto remover = [&eleMatched,&trigMatched](const EleTrigMatch& match){ return !(eleMatched.insert(std::get<0>(match)).second && trigMatched.insert(std::get<1>(match)).second);};
  eleTrigMatches_.erase(std::remove_if(eleTrigMatches_.begin(),eleTrigMatches_.end(),remover),eleTrigMatches_.end());
  if(debug){
    std::cout <<"after cleaning "<<std::endl;
    for(auto & match : eleTrigMatches_){
      auto ele = std::get<0>(match);
      auto trig = std::get<1>(match);
      auto dR = std::get<2>(match);
      std::cout <<ele->et()<<" "<<ele->detEta()<<" "<<ele->detPhi()<<" : trig "<<trig->pt()<<" "<<trig->eta()<<" "<<trig->phi()<<" : dR "<<dR<<std::endl;
    }
  }

}

void TrigFuncs::getElesPassingFilter(const SHEvent& event,const std::string& filterName,std::vector<const SHElectron*>& eles)
{
  const std::vector<const SHTrigObj*> trigObjs = event.getTrigSum().getTrigObjs(filterName);
  for(int eleNr=0;eleNr<event.nrElectrons();eleNr++){
    const SHElectron* ele = event.getElectron(eleNr);
    for(auto trigObj : trigObjs){
      if(trigObj->matches(ele->detEta(),ele->detPhi())) eles.push_back(ele);
    }
  }
}

bool TrigFuncs::filterEtIsSCEt(const std::string& filterName)
{
  if(isL1Filter(filterName)) return false;
  else if(filterName.find("TrkId")==std::string::npos) return true;
  else if(filterName.find("GsfTrkId")!=std::string::npos) return true;
  else if(filterName.find("hltEle32CaloIdTCaloIsoTTrkIdTTrkIsoTSC17PMMassFilter")!=std::string::npos) return true;
  else return false;
}

 
bool TrigFuncs::isL1Filter(const std::string& filterName)
{
  if(filterName.find("hltL1")!=std::string::npos) return true;
  else if(filterName.find("L1Single")==0) return true;
  else if(filterName.find("L1Double")==0) return true;
  else if(filterName.find("L1Triple")==0) return true;
  else return false;
}


bool TrigFuncs::isEGUnseeded(const SHTrigObj& trigObj,const SHTrigSummary& trigSum)
{
  if((trigObj.type()&SHTrigObj::EGAMMA)==0) return false;
  
  if((trigObj.type()&SHTrigObj::EGUNSEEDED)==0) return false;
    //    const TBits& bits = trigObj.filtersPassed();
    // const SHBitsDef& bitDef = trigSum.filterBitsDef();
    //for(size_t bitNr=bits.FirstSetBit();bitNr<=bits.LastSetBit();bitNr++){
      
      // if(bitDef.getBitName(bitNr).find("Unseeded")!=std::string::npos) std::cout <<" name "<<bitNr<<" "<<bitDef.getBitName(bitNr)<<" type "<<trigObj.type()<<std::endl;
  //    }
  // }
  return true;
}
bool TrigFuncs::isHIEG(const SHTrigObj& trigObj,const SHTrigSummary& trigSum)
{
  if((trigObj.type()&SHTrigObj::EGAMMA)==0) return false;

  const TBits& bits = trigObj.filtersPassed();
  const SHBitsDef& bitDef = trigSum.filterBitsDef();
  static const std::string hltStr("hltHI");
  return bitDef.getBitName(bits.FirstSetBit()).compare(0,5,hltStr)==0;
}

std::pair<const SHTrigObj*,const SHTrigObj*> TrigFuncs::getEGTrigObj(float detEta,float detPhi,const SHTrigSummary& trigSum)
{
  const SHTrigObj* bestSeeded = nullptr;
  const SHTrigObj* bestUnseeded = nullptr;
  const std::vector<const SHTrigObj*> objs = trigSum.getTrigObjs(detEta,detPhi,SHTrigObj::PHOTON);
  for(auto obj : objs){
    if(bestUnseeded==nullptr && isEGUnseeded(*obj,trigSum)){
      bestUnseeded=obj;
    }else if(bestSeeded==nullptr && !isHIEG(*obj,trigSum)) bestSeeded=obj;
  }
  return std::pair<const SHTrigObj*,const SHTrigObj*>(bestSeeded,bestUnseeded);
}


const SHTrigEcalCand* TrigFuncs::getTrigEcalCand(const SHTrigObjContainer& trigObjs,float detEta,float detPhi,float maxDR)
{
  const SHTrigEcalCand* bestCand=nullptr;
  float bestDR2=maxDR*maxDR;
  for(int objNr=0;objNr<trigObjs.nrEcalCands();objNr++){
    const SHTrigEcalCand* ecalCand = trigObjs.getEcalCand(objNr);
    float dr2 = MathFuncs::calDeltaR2(detEta,detPhi,ecalCand->eta(),ecalCand->phi());
    if(dr2<bestDR2){
      bestCand = ecalCand;
      bestDR2 = dr2;
    }
  }//end loop over all objects
  
  return bestCand;

}

//temp function to speed up the slow slow trigger code
void TrigFuncs::dropAllButRequiredTrigs(SHEvent* event)
{
  std::vector<std::string> pathNames = TrigBitsDef::trigPathNames();
  std::vector<std::string> filterNames = TrigBitsDef::trigFilterNames();
  std::sort(pathNames.begin(),pathNames.end());
  std::sort(filterNames.begin(),filterNames.end());
  

  std::vector<SHTrigInfo> trigsToKeep;

  for(int trigNr=0;trigNr<event->nrTrigs();trigNr++){
    const SHTrigInfo* trigInfo = event->getTrigInfo(trigNr);
    const std::vector<std::string>& trigNames = trigInfo->trigId()==1 ? pathNames : filterNames;
    
    if(std::binary_search(trigNames.begin(),trigNames.end(),rmTrigVersion(trigInfo->name()))){
      trigsToKeep.push_back(*trigInfo);
    }
  }
  event->clearTrigs();
  for(auto& trig : trigsToKeep) event->addTrigInfo(trig);
  
}


std::string TrigFuncs::rmTrigVersion(std::string trigname)
{
  size_t versionIndex = trigname.rfind("_v");
  
  if(versionIndex!=std::string::npos) trigname.erase(versionIndex+2);
  return trigname;
}
  

void TrigFuncs::convertOldTrigFormat(SHTrigSummary& trigSum,const SHEvent& event) 
{
  static bool warned=false;
  if(!warned){
    LogErr<<" this function is temporarily disabled, further messages are surpressed"<<std::endl;
    warned=true;
  }

  // std::vector<std::pair<SHTrigObj,TBits>> trigObjsTemp;
  // std::set<std::string> pathNames;
  // std::set<std::string> filterNames;
  
  // for(int trigNr=0;trigNr<event.nrTrigs();trigNr++){
  //   const SHTrigInfo* trigInfo  = event.getTrigInfo(trigNr);
  //   if(trigInfo->name().find("HLT_")==0){ //path
  //     pathNames.insert(rmTrigVersion(trigInfo->name()));
  //   }else filterNames.insert(trigInfo->name());
  // }

  // trigSum.setPathBitsDef(pathNames);
  // trigSum.setFilterBitsDef(filterNames);

  // const SHBitsDef& pathBitsDef = trigSum.pathBitsDef();
  // const SHBitsDef& filterBitsDef = trigSum.filterBitsDef();
  
  // for(int trigNr=0;trigNr<event.nrTrigs();trigNr++){
  //   const SHTrigInfo* trigInfo  = event.getTrigInfo(trigNr);
  //   if(trigInfo->name().find("HLT_")==0){
  //     int bitNr = pathBitsDef.getBitNr(rmTrigVersion(trigInfo->name()));
  //     auto status = trigInfo->passTrig() ? SHTrigResult::Status::PASS : SHTrigResult::Status::FAIL;
  //     trigSum.addTrigResult(SHTrigResult(bitNr,status,trigInfo->preScale()));
  //   }
  // }

  // const float maxDR2=0.0001*0.0001;
  // const float maxEtDiff=0.00001;
  // for(int trigNr=0;trigNr<event.nrTrigs();trigNr++){
  //   const SHTrigInfo* trigInfo  = event.getTrigInfo(trigNr);
  //   for(int objNr=0;objNr<trigInfo->nrPass();objNr++){
  //     const TLorentzVector& p4 = trigInfo->getObjP4(objNr);
  //     auto compFunc = [&p4,&maxDR2,&maxEtDiff](const std::pair<SHTrigObj,TBits>& lhs){return std::abs(lhs.first.pt()-p4.Pt())<maxEtDiff && MathFuncs::calDeltaR2(lhs.first.eta(),lhs.first.phi(),p4.Eta(),p4.Phi())<maxDR2;};
     
  //     auto trigObj = std::find_if(trigObjsTemp.begin(),trigObjsTemp.end(),compFunc);
  //     if(trigObj==trigObjsTemp.end()){//not found, add it
  // 	int type=SHTrigObj::PHOTON;
  // 	if(!filterEtIsSCEt(trigInfo->name())) type=SHTrigObj::ELECTRON;
  // 	if(isL1Filter(trigInfo->name())) type=SHTrigObj::L1S1EG;
  // 	//if(type==SHTrigObj::L1S1EG) LogErr <<trigInfo->name()<<" "<<isL1Filter(trigInfo->name())<<std::endl;
	
  // 	TBits bits;
  // 	trigObjsTemp.push_back({SHTrigObj(p4.Pt(),p4.Eta(),p4.Phi(),p4.Mag(),type,bits),bits});	
  // 	trigObj = trigObjsTemp.end()-1;
  //     }
  //     trigObj->second.SetBitNumber(filterBitsDef.getBitNr(trigInfo->name()));
  //     //  if(trigInfo->name()=="hltL1sL1SingleEG40") std::cout <<trigObj->first.type()<<" "<<filterBitsDef.getBitNr(trigInfo->name())<<" "<<trigObj->first.isL1S1()<<" "<<trigObj->first.isL1()<<" "<<trigObj->first.isHLT()<<std::endl;
  //   }
  // }
  
  // for(const auto& trigObj : trigObjsTemp){
  //   trigSum.addTrigObj(SHTrigObj(trigObj.first.pt(),trigObj.first.eta(),
  // 				 trigObj.first.phi(),trigObj.first.mass(),
  // 				 trigObj.first.type(),
  // 				 trigObj.second));

    
  // }
  // trigSum.sort();



  // auto eg40Objs = trigSum.getTrigObjs("hltL1sL1SingleEG40");
  // std::cout <<"eg40s "<<eg40Objs.size()<<std::endl;
  // for(int eleNr=0;eleNr<event.nrElectrons();eleNr++){
  //   const SHElectron* ele = event.getElectron(eleNr);
  //   if(ele->et()>50 && ele->hadem()<0.05){
  //     for(auto& eg40Obj : eg40Objs){
  // 	float dR2 = eg40Obj->deltaR2(ele->detEta(),ele->detPhi();
  // 	std::cout <<"L1 "<<eg40Obj->eta()<<" "<<eg40Obj->phi()<<" reco "<<ele->detEta()<<" "<<ele->detPhi()<<" dR "<<dR2<<std::endl;
  //     }
  //   }

  // }
}

std::pair<const SHTrigObj*,const SHTrigObj*> 
TrigFuncs::getL1Match(const SHTrigSummary& trigSum,float detEta,float detPhi)
{
  return getL1Match(l1FilterNames_,trigSum,detEta,detPhi);
 
}

std::pair<const SHTrigObj*,const SHTrigObj*> 
TrigFuncs::getL1Match(const std::vector<std::string>& filterNames,
		      const SHTrigSummary& trigSum,float detEta,float detPhi)
{
  auto trigObjs = trigSum.getTrigObjs(filterNames);
  auto sorter = [detEta,detPhi](const SHTrigObj* lhs,const SHTrigObj* rhs){
    return lhs->deltaR2(detEta,detPhi) < rhs->deltaR2(detEta,detPhi);
  };
  std::sort(trigObjs.begin(),trigObjs.end(),sorter);
  const SHTrigObj* eg=nullptr;
  const SHTrigObj* jet=nullptr;
  float bestEGDR2=SHTrigObj::kStdMaxDR2;
  float bestJetDR2=0.5*0.5;
  
  //  std::cout <<"starting loop "<<std::endl;
  for(auto trigObj : trigObjs){
    if(trigObj->isType(SHTrigObj::L1EG) && trigObj->deltaR2(detEta,detPhi)<bestEGDR2) {
      eg=trigObj;
      bestEGDR2=eg->deltaR2(detEta,detPhi);
    }
    if(trigObj->isType(SHTrigObj::L1JET) && trigObj->deltaR2(detEta,detPhi)<bestJetDR2){
      jet=trigObj;
      bestJetDR2=jet->deltaR2(detEta,detPhi);
    }
    //std::cout <<" pt "<<trigObj->pt()<<" "<<trigObj->eta()<<" "<<trigObj->phi()<<" type "<<trigObj->type()<<" dR2 "<<trigObj->deltaR2(detEta,detPhi)<<std::endl;
  }
  return {eg,jet};
}
const SHTrigObj*
TrigFuncs::getL1SingleMatch(const std::vector<std::string>& filterNames,
		      const SHTrigSummary& trigSum,float detEta,float detPhi)
{
  auto trigObjs = trigSum.getTrigObjs(filterNames);
  auto sorter = [detEta,detPhi](const SHTrigObj* lhs,const SHTrigObj* rhs){
    return lhs->deltaR2(detEta,detPhi) < rhs->deltaR2(detEta,detPhi);
  };
  std::sort(trigObjs.begin(),trigObjs.end(),sorter);
  const SHTrigObj* tau=nullptr;

  float bestTauDR2=SHTrigObj::kStdMaxDR2;
  for(auto trigObj : trigObjs){
    //std::cout <<"trigObj "<<trigObj->type()<<" et "<<trigObj->pt()<<" "<<trigObj->deltaR2(detEta,detPhi)<<std::endl;
    if(trigObj->isType(SHTrigObj::L1TAU) && trigObj->deltaR2(detEta,detPhi)<bestTauDR2) {
      tau=trigObj;
      bestTauDR2=tau->deltaR2(detEta,detPhi);
    }
    
  }
  //std::cout <<"tau "<<tau<<std::endl;
  return tau;
}


std::vector<std::pair<int,std::vector<int> > > TrigFuncs::createEG26PSLookup()
{
  std::vector<std::pair<int,std::vector<int> > > eg26PSets;
  eg26PSets.push_back({284078,{1,1,1,1,1}});
  eg26PSets.push_back({284077,{1,1,1,1,1}});
  eg26PSets.push_back({284044,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284043,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284042,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284041,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284040,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284039,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284038,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284037,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284036,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284035,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284029,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284025,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284014,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({284006,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283964,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283946,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283934,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283933,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283885,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283884,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283878,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283877,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283876,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283865,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283863,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283835,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283834,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283830,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283820,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283685,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283682,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283681,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283680,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283676,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283675,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283672,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283548,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283478,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283469,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283453,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283416,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283415,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283414,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283413,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283408,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283407,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283387,{1,1,1,1,1}});
  eg26PSets.push_back({283359,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283358,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283353,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283308,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283307,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283306,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283305,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283283,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283270,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283171,{0,0,0,0,0,0,0,400,400,400,400}});
  eg26PSets.push_back({283067,{550,500,450,400,350,300,210,170,150,150,150,150,150,150,100,80}});
  eg26PSets.push_back({283059,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283052,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283050,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283049,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283043,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283042,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({283041,{550,500,450,400,350,300,210,170,150,150,150,150,150,150,100,80}});
  eg26PSets.push_back({283040,{550,500,450,400,350,300,210,170,150,150,150,150,150,150,100,80}});
  eg26PSets.push_back({282924,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282923,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282922,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282921,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282920,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282919,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282918,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282917,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282842,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282814,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282807,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282800,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282799,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282735,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282734,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282733,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282732,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282731,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282730,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282712,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282711,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282710,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282709,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282708,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282707,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282663,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282650,{400,400,400,400,400,400,400,400}});
  eg26PSets.push_back({282649,{400,400,400,400,400,400,400,400}});
  eg26PSets.push_back({282408,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282092,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282037,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282035,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282034,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({282033,{550,500,450,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281976,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281975,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281974,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281797,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281727,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281726,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281707,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281693,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281691,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281689,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281686,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281685,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281680,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281674,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281671,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281663,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281641,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281639,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281638,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281616,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281613,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4}});
  eg26PSets.push_back({281602,{500,400,350,300,210,170,150,100,80,40,20,15,10,4,2,0}});
  eg26PSets.push_back({280385,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280384,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280383,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280364,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280363,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280361,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280349,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280330,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280327,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280251,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280249,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280242,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280241,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280240,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280239,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280238,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280194,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280192,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280191,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280190,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280189,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280188,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280187,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280024,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280023,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280022,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280021,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280020,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280019,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280018,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280017,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280016,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280015,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280014,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280013,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280007,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280006,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280002,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({280001,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279995,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279994,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279993,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279975,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279966,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279931,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279887,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279865,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279864,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279862,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279861,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279860,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279859,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279858,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279857,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279856,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279855,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279854,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279853,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279852,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279851,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279850,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279849,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279848,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279846,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279845,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279844,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279841,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,15,15,15}});
  eg26PSets.push_back({279823,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279794,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279767,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279766,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279760,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279716,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279715,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279694,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279691,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279685,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279684,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279683,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279682,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279681,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279667,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279658,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279657,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279656,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279654,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279653,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279588,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279489,{150,150,150,150,150,150,0}});
  eg26PSets.push_back({279488,{150,150,150,150,150,150,0}});
  eg26PSets.push_back({279487,{150,150,150,150,150,150,0}});
  eg26PSets.push_back({279484,{150,150,150,150,150,150,0}});
  eg26PSets.push_back({279480,{150,150,150,150,150,150,0}});
  eg26PSets.push_back({279479,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279116,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279115,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279080,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279073,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279072,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279071,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279029,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279028,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({279024,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278986,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278976,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278975,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278969,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278963,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278962,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278957,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278923,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278875,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278874,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278873,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278822,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278821,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278820,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278808,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278807,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278806,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278805,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278804,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278803,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278802,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278801,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278770,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278769,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278765,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278761,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278509,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278406,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278366,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278349,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278346,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278345,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278315,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278310,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278309,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278308,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278290,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278289,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278288,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278274,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278273,{500,400,350,300,210,170,150,100,80,40,20,15,10,8,4,2,0}});
  eg26PSets.push_back({278240,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({278239,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({278193,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({278175,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({278167,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({278018,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({278017,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277992,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277991,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277990,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277985,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277984,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277983,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277982,{90,90,90,90,90}});
  eg26PSets.push_back({277981,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277935,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277934,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277933,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277932,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277420,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277305,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277220,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277219,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277218,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277217,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277202,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277194,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277180,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277168,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277166,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277148,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277127,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277126,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277112,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277096,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277094,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277093,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277087,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277086,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277081,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277076,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277075,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277073,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277072,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277071,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277070,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({277069,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276950,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276948,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276947,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276946,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276945,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276944,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276942,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276941,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276940,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276935,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276870,{500,400,350,300,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276837,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276836,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276835,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276834,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276832,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276831,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276811,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276810,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276808,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276807,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276794,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276776,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276775,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276659,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276655,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276653,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276587,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276586,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276585,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276584,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276583,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276582,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276581,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276545,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276544,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276543,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276542,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276528,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276527,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276525,{350,250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276502,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276501,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276495,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276458,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276457,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276456,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276455,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276454,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276453,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276437,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276384,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276363,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276361,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276357,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276355,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276352,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276327,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276326,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276318,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276317,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276315,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276283,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276282,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276244,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276243,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276242,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276237,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276235,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276232,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276229,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276226,{0,0,0,0,0,0,0,0,0,0,0,0,0}});
  eg26PSets.push_back({276225,{0,0,0,0,0,0,0,0,0,0,0,0,0}});
  eg26PSets.push_back({276217,{0,0,0,0,0,0,0,0,0,0,0,0,0}});
  eg26PSets.push_back({276216,{0,0,0,0,0,0,0,0,0,0,0,0,0}});
  eg26PSets.push_back({276215,{0,0,0,0,0,0,0,0,0,0,0,0,0}});
  eg26PSets.push_back({276097,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276095,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276093,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276092,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276072,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276071,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276064,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276063,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276062,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({276054,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275963,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275931,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275923,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275922,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275921,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275920,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275918,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275913,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275912,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275911,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275890,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275887,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275886,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275847,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275846,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275841,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275838,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275837,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275836,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275835,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275834,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275833,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275832,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275831,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275829,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275828,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275783,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275782,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275781,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275778,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275777,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275776,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275774,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275773,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275772,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275769,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275768,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275767,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275766,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275764,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275763,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275761,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275759,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275758,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275757,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275659,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275658,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275657,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275656,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275376,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275375,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275371,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275370,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275345,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275344,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275338,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275337,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275326,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275319,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275311,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275310,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275309,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275293,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275292,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275291,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275290,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275289,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275286,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275285,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275284,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275283,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275282,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275125,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275124,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275074,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275073,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275068,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275067,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275066,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275064,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275063,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275062,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275059,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275001,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({275000,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274999,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274998,{250,210,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274971,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274970,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274969,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274968,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274967,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274966,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274959,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274958,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274957,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274956,{50,50,50,50,50}});
  eg26PSets.push_back({274955,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274954,{250,250,180,120,90,50,1,1,1,1,1,1,50,50,0}});
  eg26PSets.push_back({274443,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274442,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274441,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274440,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274422,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274421,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274420,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274388,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274387,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274382,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274345,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274344,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274339,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274338,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274337,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274336,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274335,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274319,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274318,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274317,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274316,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274315,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274314,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274286,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274285,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274284,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274283,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274282,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274251,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274250,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274244,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274243,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274241,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274240,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274200,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274199,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274198,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274172,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274161,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274160,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274159,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274157,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274146,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274142,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({274108,{1,1,1,1,1,1}});
  eg26PSets.push_back({274107,{1,1,1,1,1,1}});
  eg26PSets.push_back({274106,{1,1,1,1,1,1}});
  eg26PSets.push_back({274105,{1,1,1,1,1,1}});
  eg26PSets.push_back({274104,{1,1,1,1,1,1}});
  eg26PSets.push_back({274103,{1,1,1,1,1,1}});
  eg26PSets.push_back({274102,{1,1,1,1,1,1}});
  eg26PSets.push_back({274100,{0,0,0}});
  eg26PSets.push_back({274094,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273730,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273728,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273725,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273593,{0,0,0}});
  eg26PSets.push_back({273592,{1,1,1,1,1,1}});
  eg26PSets.push_back({273591,{0,0,0}});
  eg26PSets.push_back({273590,{1,1,1,1,1,1}});
  eg26PSets.push_back({273589,{1,1,1,1,1,1}});
  eg26PSets.push_back({273588,{0,0,0}});
  eg26PSets.push_back({273555,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273554,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273537,{1,1,1,1,1,1}});
  eg26PSets.push_back({273531,{0,0,0}});
  eg26PSets.push_back({273526,{1,1,1,1,1,1}});
  eg26PSets.push_back({273523,{1,1,1,1,1,1}});
  eg26PSets.push_back({273522,{0,0,0}});
  eg26PSets.push_back({273504,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273503,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273502,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273494,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273493,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273492,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273450,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273449,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273448,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273447,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273446,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273445,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273426,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273425,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273412,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273411,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273410,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273409,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273408,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273407,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273406,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273405,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273404,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273403,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273402,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273302,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273301,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273299,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273296,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273295,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273294,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273292,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273291,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273290,{0,250,180,120,90,50,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273158,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273150,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273063,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273017,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({273013,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272936,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272935,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272930,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272927,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272926,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272925,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272924,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272923,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272922,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272828,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272827,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272818,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272816,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272815,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272814,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272812,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272811,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272798,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272787,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272786,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272785,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272784,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272783,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272782,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272776,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272775,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272774,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272762,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272761,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272760,{0,1500,1000,700,500,300,1,1,1,1,1,1,0}});
  eg26PSets.push_back({272022,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272021,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272019,{0,1500,1000,700,500,300,150,100,70,50,30,15,0}});
  eg26PSets.push_back({272018,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272017,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272016,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272014,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272012,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272011,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272010,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272008,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272007,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  eg26PSets.push_back({272006,{0,1500,1000,700,500,300,150,100,70,50,30,0,15,0,0}});
  std::sort(eg26PSets.begin(),eg26PSets.end(),TempFuncs::PairSortBy1st<int,std::vector<int> >());
  return eg26PSets;
}


int TrigFuncs::eg26PS(int runnr,size_t psColumn)
{
  if(runnr==1) return 1; //MC
  auto result = std::equal_range(l1EG26Prescales_.begin(),l1EG26Prescales_.end(),runnr,TempFuncs::PairSortBy1st<int,std::vector<int> >());
  int nrFound =std::distance(result.first,result.second);
  if(nrFound==0){
    LogErr<<" runnr "<<runnr<<" not found"<<std::endl;
    return 1;
  }else if(nrFound>1){
    LogErr<<nrFound<<" entries for runnr "<<runnr<<" found"<<std::endl;
  }
  auto& psSet = *result.first;
  
  if(psColumn<psSet.second.size()) return psSet.second[psColumn];
  else{
    LogErr<<" ps column "<<psColumn<<" is out of bounds (nr colums "<<psSet.second.size()<<" )"<<std::endl;
    return 1;
  }
}
