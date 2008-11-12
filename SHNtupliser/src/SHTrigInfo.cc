#include "SHarper/SHNtupliser/interface/SHTrigInfo.hh"

#include "SHarper/SHNtupliser/interface/MathFuncs.hh"


ClassImp(SHTrigInfo)

bool SHTrigInfo::passTrig(double eta,double phi)const
{
  double minDeltaR2=0.05*0.05;

  for(int objNr=0;objNr<nrPass();objNr++){
    double deltaR2 = MathFuncs::calDeltaR2(eta,phi,getObjP4(objNr));
    if(deltaR2<minDeltaR2) return true; //found a trigger object well matched in dR, input object passed
  }
  //didnt find an object well matched in dR, input object failed
  return false;
}
