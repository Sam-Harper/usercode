#include "SHarper/SHNtupliser/interface/SHTrigInfo.hh"

#include "SHarper/SHNtupliser/interface/MathFuncs.hh"


ClassImp(SHTrigInfo)

bool SHTrigInfo::passTrig(double eta,double phi)const
{
  // if(!pass_) return false; //can never pass if trigger didnt accept

  double minDeltaR2=0.1*0.1;
  //  std::cout <<"here "<<std::endl;
  for(int objNr=0;objNr<nrPass();objNr++){
    //   std::cout <<"objNr "<<objNr<<" / "<<nrPass()<<std::endl;
    double deltaR2 = MathFuncs::calDeltaR2(eta,phi,getObjP4(objNr));
    if(deltaR2<minDeltaR2) return true; //found a trigger object well matched in dR, input object passed
  }
  //didnt find an object well matched in dR, input object failed
  return false;
}

TLorentzVector SHTrigInfo::getTrigObj(double eta,double phi)const
{
   double minDeltaR2=0.1*0.1;
  //  std::cout <<"here "<<std::endl;
  for(int objNr=0;objNr<nrPass();objNr++){
    //   std::cout <<"objNr "<<objNr<<" / "<<nrPass()<<std::endl;
    double deltaR2 = MathFuncs::calDeltaR2(eta,phi,getObjP4(objNr));
    if(deltaR2<minDeltaR2) return getObjP4(objNr); //found a trigger object well matched in dR, input object passed
  }
  //didnt find an object well matched in dR, input object failed
  return TLorentzVector(-999,-999,-999,-999);

}

//l1 triggers have discrete eta/phi, need to special matching
//stolen from HLTEgammaL1MatchFilterRegional
bool SHTrigInfo::passL1Trig(double eta,double phi)const
{
  // if(!pass_) return false; //can never pass if trigger didnt accept
  
  const double barrelEnd=1.4791;
  // const double endcapEnd=2.65;
  const double regionEtaSizeEB=0.522;
  const double regionEtaSizeEE=1.0;
  const double regionPhiSize=1.044;

  for(int objNr=0;objNr<nrPass();objNr++){
    //ORCA matching method
    double etaBinLow  = 0.;
    double etaBinHigh = 0.;

    double objEta = getObjP4(objNr).Eta();
    double objPhi = getObjP4(objNr).Phi(); 
    
    if(fabs(objEta) < barrelEnd){
      etaBinLow = objEta - regionEtaSizeEB/2.;
      etaBinHigh = etaBinLow + regionEtaSizeEB;
    }
    else{
      etaBinLow = objEta - regionEtaSizeEE/2.;
      etaBinHigh = etaBinLow + regionEtaSizeEE;
    }
    
    float deltaPhi=MathFuncs::deltaPhi(phi,objPhi);
   
    
    if(eta < etaBinHigh && eta > etaBinLow &&
       deltaPhi <regionPhiSize/2. )  {
      return true;
    }
    
  }
 
  //didnt find an object well matched in dR, input object failed
  return false;
}
