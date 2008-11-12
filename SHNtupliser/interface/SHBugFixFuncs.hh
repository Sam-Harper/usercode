#ifndef SHBUGFIXFUNCS
#define SHBUGFIXFUNCS

//occasionly, high standards though there are (and if you believe that I've a bridge to sell you), bugs slip through the cracks
//these functions fix them, usually correct something at the ntuple level

#include "SHarper/SHNtupliser/interface/SHElectron.hh"
#include "SHarper/SHNtupliser/interface/SHEvent.hh"

namespace SHBugFixFuncs {
  //for v7 from HEEP AOD, the objects from the high et and very high et trigger were accidently not stored
  //this code sees if any electrons would have passed those triggers
  
  // bool passHighEtTrig(const SHElectron& ele);
  //bool passVeryHighEtTrig(const SHElectron& ele);
  const SHElectron* matchEle(double eta,double phi,const SHEvent* event);

}

#endif


