#include "SHarper/SHNtupliser/interface/SHTrigEcalCand.hh"

#include "SHarper/SHNtupliser/interface/TempFuncs.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"

ClassImp(SHTrigEcalCand)

void SHTrigEcalCand::setVars(std::vector<std::pair<std::string,float> >& inputVar)
{
  vars_.swap(inputVar);
  std::sort(vars_.begin(),vars_.end(),TempFuncs::PairComp<std::string,float,std::less<std::string> >());
  

}

float SHTrigEcalCand::var(const std::string& varName)const
{
  typedef std::vector<std::pair<std::string,float> >::const_iterator ConstIt;
  std::pair<ConstIt,ConstIt> result =  std::equal_range(vars_.begin(),vars_.end(),varName,TempFuncs::PairComp<std::string,float,std::less<std::string> >());
  
  if(result.second-result.first==1){
    return result.first->second;
  }else if(result.second-result.first==0){
    return -999;
  }else{
    LogErr << " Error,  "<<result.second-result.first<<" keys match "<<varName<<std::endl;
    return -999;
  }  

}
