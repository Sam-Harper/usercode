#include "SHarper/SHNtupliser/interface/SHTrigEcalCand.hh"

#include "SHarper/SHNtupliser/interface/TempFuncs.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"

#include <iostream>

ClassImp(SHTrigEcalCand)


void SHTrigEcalCand::setVars(std::vector<std::pair<std::string,float> >& inputVar)
{
  vars_.swap(inputVar);
  std::sort(vars_.begin(),vars_.end(),TempFuncs::PairComp<std::string,float,std::less<std::string> >());
  

}

float SHTrigEcalCand::var(const std::string& varName)const
{
  if(varName=="et") return et();
  else if(varName=="eta") return eta();  
  else if(varName=="etaAbs") return fabs(eta());
  else if(varName=="phi") return phi();
  else if(varName=="nrgy") return nrgy();

  typedef std::vector<std::pair<std::string,float> >::const_iterator ConstIt;
  std::pair<ConstIt,ConstIt> result =  std::equal_range(vars_.begin(),vars_.end(),varName,TempFuncs::PairComp<std::string,float,std::less<std::string> >());
  
  if(result.second-result.first==1){
    return result.first->second;
  }else if(result.second-result.first==0){

    LogErr <<" Warning, var "<<varName<<" not found "<<std::endl;
    return -999;
  }else{
    LogErr << " Error,  "<<result.second-result.first<<" keys match "<<varName<<std::endl;
    return -999;
  }  

}


std::ostream & SHTrigEcalCand::print(std::ostream& output)const
{
  output<<"energy "<<p4_.E()<<" et "<<p4_.Et()<<" eta "<<p4_.Eta()<<" phi "<<p4_.Phi()<<std::endl;
  for(size_t varNr=0;varNr<vars_.size();varNr++){
    output<<vars_[varNr].first<<" "<<vars_[varNr].second<<std::endl;
  }
  return output;
}

std::ostream &operator <<(std::ostream& output,const SHTrigEcalCand &cand){return cand.print(output);}
