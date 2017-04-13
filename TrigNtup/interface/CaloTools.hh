#ifndef UTILITY_CALOTOOLS_HH
#define UTILITY_CALOTOOLS_HH

#include <cmath>

class CaloTools {

public:

#ifndef __CINT__  //hiding the nasty c++11 from poor cint
  constexpr static float kMaxEtaEB=1.4442;
  constexpr static float kMinEtaEE=1.566;
  constexpr static float kMaxEtaEE=2.5;
#endif

private:
  CaloTools(){} //would delete it but cint needs it
public:
  static int region(float eta){
    float etaAbs = std::abs(eta);
    return etaAbs<=kMaxEtaEB ? 0 : etaAbs>=kMinEtaEE && etaAbs<=kMaxEtaEE ? 1 : 10;
  }
  static int evtRegion(float eta1,float eta2){return region(eta1)+region(eta2);}

};


#endif
