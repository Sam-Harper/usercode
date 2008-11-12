#include "SHarper/SHNtupliser/interface/SHBugFixFuncs.hh"

#include "SHarper/SHNtupliser/interface/MathFuncs.hh"


 
const SHElectron* SHBugFixFuncs::matchEle(double eta,double phi,const SHEvent* event)
{  
  if(event==NULL) return NULL;

  const SHElectron* bestMatchEle = NULL;
  double bestDeltaR2=99999999.;
  for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
    const SHElectron* ele = event->getElectron(eleNr);
    double deltaR2 = MathFuncs::calDeltaR2(eta,phi,ele->eta(),ele->phi());
    if(deltaR2<bestDeltaR2){
      bestMatchEle = ele;
      bestDeltaR2 = deltaR2;
    }
  }
  return bestMatchEle;
}
  
