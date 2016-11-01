#include "SHarper/SHNtupliser/interface/SHTrigSummary.hh"


#include "SHarper/SHNtupliser/interface/LogErr.hh"
ClassImp(SHTrigSummary)

const SHL1Result SHTrigSummary::nullL1Result_;
const SHTrigResult SHTrigSummary::nullTrigResult_;
const SHTrigObj SHTrigSummary::nullTrigObj_;
const SHBitsDef SHTrigSummary::nullBitsDef_;

SHL1Result SHTrigSummary::getL1Trig(const std::string& seedName)const
{
  if(l1Menu_==nullptr) return nullL1Result_;
  
  const SHL1Menu::Seed& seed = l1Menu_->getSeed(seedName);
  if(!seed.valid()) return nullL1Result_;
  
  bool accept = l1Result_.TestBitNumber(seed.bitNr());
  //its important to take seedName not seed.name() incase we have a null seed (ie we didnt find the seed)
  return SHL1Result(seed.bitNr(),seedName,accept,seed.masked(),seed.preScale(preScaleColumn()));
}

SHTrigResult SHTrigSummary::getTrig(const std::string& trigName)const
{
  if(hltMenu_==nullptr) return nullTrigResult_;
  
  const SHHLTMenu::Path& path = hltMenu_->getPath(trigName);
  if(!path.valid()) return nullTrigResult_;
  
  SHTrigResult::Status status = hltResult_.TestBitNumber(path.pathNr()) ? 
    SHTrigResult::Status::PASS : SHTrigResult::Status::FAIL;
    
  int preScale = path.preScale(preScaleColumn());
  
  std::vector<SHL1Result> l1Seeds;
  for(auto l1SeedName : path.l1Seeds()){
    SHL1Result l1Res = getL1Trig(l1SeedName);
    l1Seeds.emplace_back(std::move(l1Res));
  }
  //its important to take trigName not path.name() incase we have a null seed (ie we didnt find the seed)
  return SHTrigResult(path.pathNr(),trigName,status,preScale,std::move(l1Seeds));
}



const SHTrigObj& SHTrigSummary::getTrigObj(const std::string& trigName,float eta,float phi,float maxDR)const
{
  std::vector<std::string> trigNames={trigName};
  return getTrigObj(trigNames,eta,phi,maxDR);
}
  

const SHTrigObj& SHTrigSummary::getTrigObj(const std::vector<std::string>& trigNames,
					   float eta,float phi,float maxDR)const
{
  TBits bits = filterBitsDef().getBits(trigNames);
  auto allTrigObjs = getTrigObjs(eta,phi,SHTrigObj::ALL,maxDR);
  std::vector<const SHTrigObj*> passingTrigObjs;
  std::copy_if(allTrigObjs.begin(),allTrigObjs.end(),std::back_inserter(passingTrigObjs),
	       [bits](const SHTrigObj* obj){return obj->pass(bits);});
  if(passingTrigObjs.empty()) return nullTrigObj_;
  else{
    std::partial_sort(passingTrigObjs.begin(),passingTrigObjs.begin()+1,passingTrigObjs.end(),
		      [eta,phi](const SHTrigObj* lhs,const SHTrigObj* rhs){
			return lhs->deltaR2(eta,phi)<rhs->deltaR2(eta,phi);});
    return *passingTrigObjs.front();
  }
			  
}

std::vector<const SHTrigObj*> SHTrigSummary::getTrigObjs(const std::string& trigName)const
{
  std::vector<std::string> trigNames={trigName};
  return getTrigObjs(trigNames);
}

std::vector<const SHTrigObj*> SHTrigSummary::getTrigObjs(const std::vector<std::string>& trigNames)const
{
  TBits bits = filterBitsDef().getBits(trigNames);
  std::vector<const SHTrigObj*> passingTrigObjs;
  for(auto& trigObj : trigObjs()){
    if(trigObj.pass(bits)) passingTrigObjs.push_back(&trigObj);
  }
  return passingTrigObjs;
}
			 

std::vector<const SHTrigObj*> SHTrigSummary::getTrigObjs(float eta,float phi,int type,float maxDR)const
{
  const float maxDR2 = maxDR*maxDR;
  std::vector<const SHTrigObj*> matchedObjs;
  for(const SHTrigObj& trigObj : trigObjs()){
    if( (trigObj.type()&type)!=0 &&
	trigObj.deltaR2(eta,phi)<maxDR2 ){
      matchedObjs.push_back(&trigObj);
    }
  } 
  return matchedObjs;
}

std::vector<SHTrigObj*> SHTrigSummary::getTrigObjs(float eta,float phi,int type,float maxDR)
{
  const float maxDR2 = maxDR*maxDR;
  std::vector<SHTrigObj*> matchedObjs;
  for(SHTrigObj& trigObj : trigObjs()){
    if( (trigObj.type()&type)!=0 &&
	trigObj.deltaR2(eta,phi)<maxDR2 ){
      matchedObjs.push_back(&trigObj);
    }
  } 
  return matchedObjs;
}

void SHTrigSummary::print()const
{
  printTrigs();
  printObjs();
}

void SHTrigSummary::printTrigs()const
{
  std::cout <<"Menu "<<menuName()<<" process "<<processName()<<" gt "<<(hltMenu_ ? hltMenu_->globalTag() : "")<<std::endl;
  std::cout <<"nr trigs "<<hltResult_.GetNbits()<<std::endl;
  std::cout <<"nr objs "<<trigObjs().size()<<std::endl;
  if(hltMenu_){
    for(auto& path : hltMenu_->paths()){
      std::cout <<"trig "<<path.pathNr()<<" "<<path.name()<<" pass: "<<hltResult_.TestBitNumber(path.pathNr())<<" ps: "<<path.preScale(preScaleColumn())<<std::endl;
    }
  }else std::cout <<"no HLT menu data"<<std::endl;
}
void SHTrigSummary::printObjs()const{
  
  for(auto& trig : trigObjs()){
    std::cout <<"trig "<<std::hex<<trig.type()<<std::dec<<" "<<trig.pt()<<" "<<trig.eta()<<" "<<trig.phi()<<" "<<trig.mass();
    const std::vector<std::string> names = filterBitsDef().getBitNames(trig.filtersPassed());
    for(auto& name : names) std::cout <<" "<<name;
    std::cout <<std::endl;
  }

}

void SHTrigSummary::clearEvent()
{
  preScaleColumn_=-1;
  trigObjs_.clear();
  l1Result_.Clear();
  hltResult_.Clear();
  
}

void SHTrigSummary::clearMenu()
{
  l1MenuName_="";
  menuName_="";
  processName_="";
  clearMenuData();
}

void SHTrigSummary::clearMenuData()
{
  hltMenu_=nullptr;
  l1Menu_=nullptr;
}

void SHTrigSummary::sort()
{
  std::sort(trigObjs_.begin(),trigObjs_.end(),SHTrigObjComp());
}

