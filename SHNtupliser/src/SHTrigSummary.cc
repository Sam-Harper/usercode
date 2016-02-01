#include "SHarper/SHNtupliser/interface/SHTrigSummary.hh"


#include "SHarper/SHNtupliser/interface/LogErr.hh"
ClassImp(SHTrigSummary)

const SHTrigResult SHTrigSummary::nullTrigResult_;
const SHTrigObj SHTrigSummary::nullTrigObj_;


const SHTrigResult& SHTrigSummary::getTrig(const std::string& trigName)const
{
  size_t bitNr=pathBitsDef_.getBitNr(trigName);
  auto result=std::equal_range(trigResults_.begin(),trigResults_.end(),bitNr,SHTrigResult::Sorter<>());
  if(result.first!=result.second){
    if(std::distance(result.first,result.second)!=1){
      LogErr<<" warning "<<std::distance(result.first,result.second)<<" entries found for trigger "<<trigName<<" this should not be possible"<<std::endl;
    }
    return *result.first;
  }
  else return nullTrigResult_;
}

const SHTrigObj& SHTrigSummary::getTrigObj(const std::string& trigName,float eta,float phi,float maxDR)const
{
  TBits bits = filterBitsDef_.getBits(trigName);
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
  TBits bits = filterBitsDef_.getBits(trigName);
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

void SHTrigSummary::clearEvent()
{
  preScaleColumn_=-1;
  trigObjs_.clear();
  trigResults_.clear();

}

void SHTrigSummary::clearMenu()
{
  menuName_="";
  processName_="";
  globalTag_="";
  pathBitsDef_.clear();
  filterBitsDef_.clear();
}

void SHTrigSummary::sort()
{
  std::sort(trigObjs_.begin(),trigObjs_.end(),SHTrigObjComp());
  std::sort(trigResults_.begin(),trigResults_.end());

}

