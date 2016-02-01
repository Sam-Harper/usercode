#include "SHarper/SHNtupliser/interface/SHTrigObj.hh"
#include "SHarper/SHNtupliser/interface/MathFuncs.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"
#include <array>

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

float SHTrigObj::deltaR2(float rhsEta,float rhsPhi)const
{ 
  if(isHLT() || isL1()){
    return MathFuncs::calDeltaR2(eta_,phi_,rhsEta,rhsPhi);
  }else if(isL1S1()){
    return deltaR2L1_(rhsEta,rhsPhi);
  }else{
    LogErr <<" unrecongised trigger type for DR "<<type_<<std::endl;
    return 999.;
  }
}

//l1 triggers have discrete eta/phi, need to special matching
float SHTrigObj::deltaR2L1_(float rhsEta,float rhsPhi)const
{

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
