#include "SHarper/SHNtupliser/interface/SHTrigObj.hh"
#include "SHarper/SHNtupliser/interface/MathFuncs.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"
#include <array>
#include <limits>
ClassImp(SHTrigObj)


SHTrigObj::SHTrigObj(float iPt,float iEta,float iPhi,float iMass,
		     int iType,const TBits& iFiltersPassed):
  pt_(iPt),eta_(iEta),phi_(iPhi),mass_(iMass),
  type_(iType),filtersPassed_(iFiltersPassed)
{
}




bool SHTrigObj::pass(const TBits& bits)const
{
  //so we will allow miss-matching number of bits padding the smaller one with effectively zero
  //which means it can never AND together
  size_t maxBitNr = std::min(filtersPassed_.GetNbits(),bits.GetNbits());
  for(size_t bitNr=0;bitNr<maxBitNr;bitNr++){
    if(bits.TestBitNumber(bitNr) && filtersPassed_.TestBitNumber(bitNr)) return true;
  }
  return false;
}

float SHTrigObj::var(const std::string& varName)const
{
  // if(varName=="pt") return pt();
  // else if(varName=="eta") return eta();  
  // else if(varName=="etaAbs") return fabs(eta());
  // else if(varName=="phi") return phi();
  // else if(varName=="nrgy") return p4().E();
  // else if(varName=="et") return p4().Et();
  if(vars_.empty()) return std::numeric_limits<float>::max();

  auto result =  std::equal_range(vars_.begin(),vars_.end(),varName,VarSorter());
  
  if(result.second-result.first==1){
    return result.first->second;
  }else if(result.second-result.first==0){
    // now suppress the error message as the vars wont always be present
    //    LogErr <<" Warning, var "<<varName<<" not found "<<std::endl;
    return std::numeric_limits<float>::max();
  }else{
    LogErr << " Error,  "<<result.second-result.first<<" keys match "<<varName<<std::endl;
    return std::numeric_limits<float>::max();
  }  
}

void SHTrigObj::addVars(const std::vector<std::pair<std::string,float> >& vars)
{
  vars_.insert(vars_.end(),vars.begin(),vars.end());
  sort_();
}

void SHTrigObj::addVars(std::vector<std::pair<std::string,float> >&& vars)
{
  vars_.insert(vars_.end(),std::make_move_iterator(vars.begin()),std::make_move_iterator(vars.end()));
  sort_();
}

std::ostream& SHTrigObj::print(std::ostream& out)const
{
  out <<"pt "<<pt_<<" eta "<<eta_<<" phi "<<phi_<<" mass "<<mass_<<" type "<<type_<<" debug var: "<<vars_.size()<<std::endl;
  for(auto& var : vars_){
    std::cout <<"     "<<var.first<<" "<<var.second<<std::endl;
  }
  return out;
}

float SHTrigObj::deltaR2(float rhsEta,float rhsPhi)const
{ 
  if(isHLT() || isL1()){
    return MathFuncs::calDeltaR2(eta_,phi_,rhsEta,rhsPhi);
  }else if(isL1S1()){
    return deltaR2L1_(rhsEta,rhsPhi);
  }else if(isUndefined()){
    return MathFuncs::calDeltaR2(eta_,phi_,rhsEta,rhsPhi);
  }else{
    LogErr <<" unrecongised trigger type for DR "<<type_<<std::endl;
    return 999.;
  }
}

bool SHTrigObj::passL1CMSSWMatch(float rhsEta,float rhsPhi)const
{  
  bool passed=false;

  const double barrelEnd=1.4791;
  // const double endcapEnd=2.65;
  const double regionEtaSizeEB=0.522;
  const double regionEtaSizeEE=1.0;
  const double regionPhiSize=1.044;

  //ORCA matching method
  double etaBinLow  = 0.;
  double etaBinHigh = 0.;
  
  if(fabs(eta()) < barrelEnd){
    etaBinLow = eta() - regionEtaSizeEB/2.;
    etaBinHigh = etaBinLow + regionEtaSizeEB;
  }
  else{
    etaBinLow = eta() - regionEtaSizeEE/2.;
    etaBinHigh = etaBinLow + regionEtaSizeEE;
  }
  
  float deltaPhi=MathFuncs::deltaPhi(rhsPhi,phi());
   
  if(rhsEta < etaBinHigh && rhsEta > etaBinLow &&
     deltaPhi <regionPhiSize/2. )  {
    passed=true;
  }
  
  return passed;
}



//l1 triggers have discrete eta/phi, need to special matching
float SHTrigObj::deltaR2L1_(float rhsEta,float rhsPhi)const
{
  //just trying to get it relatively close to same scales
  if(rhsPhi-phi_ > 1.5*MathFuncs::kPi) rhsPhi-=2*MathFuncs::kPi;
  if(phi_ - rhsPhi > 1.5*MathFuncs::kPi) rhsPhi+=2*MathFuncs::kPi;
  
  constexpr std::array<float,17> etaBins{{-6.000,-3.000,-2.170,-1.740,-1.392,-1.044,-0.695,-0.348,
	0.,0.348,0.695,1.044,1.392,1.740,2.170,3.000,6.000}};
  constexpr std::array<float,20> phiBins={{-3.31612,-2.96706,-2.61799,-2.26893,-1.91986,-1.5708,-1.22173,-0.872664,-0.523599,-0.174533,0.174533,0.523599,0.872665,1.22173,1.5708,1.91986,2.26893,2.61799,2.96706,3.31613}};
  
  auto upperEtaBinEdge=std::upper_bound(etaBins.begin(),etaBins.end(),eta_);
  std::pair<float,float> etaBin(*(upperEtaBinEdge-1),*upperEtaBinEdge);
  
  auto upperPhiBinEdge=std::upper_bound(phiBins.begin(),phiBins.end(),phi_);
  std::pair<float,float> phiBin(*(upperPhiBinEdge-1),*upperPhiBinEdge);
 
  //not going to be too careful about the sign here
  //also its 0 if its in the bin, dEta is the distance from bin eta
  float dEta=0;
  if(rhsEta<etaBin.first) dEta=etaBin.first-rhsEta;
  else if(rhsEta>etaBin.second) dEta=rhsEta-etaBin.second;
 
  float dPhi=0;
  if(rhsPhi<phiBin.first || rhsPhi>phiBin.second){
    float dPhiBinEdge1=MathFuncs::deltaPhi(rhsPhi,phiBin.first);
    float dPhiBinEdge2=MathFuncs::deltaPhi(rhsPhi,phiBin.second);
    dPhi = std::min(std::abs(dPhiBinEdge1),std::abs(dPhiBinEdge2));
  }

  return dEta*dEta+dPhi*dPhi;

}
