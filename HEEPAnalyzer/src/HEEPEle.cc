#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"
 
const pat::Electron& heep::Ele::patEle()const
{
  if(isPatEle()) return *patEle_;
  else {
    throw cms::Exception("InvalidReference","Electron is not a PAT electron");
  }
}

float heep::Ele::sigmaEtaEtaFull5x5()const
{
  if(fabs(scEta())<1.479) return gsfEle_->sigmaEtaEta(); //barrel case, no correction
  else{ //endcap, need to apply eta correction
    return gsfEle_->sigmaEtaEta() - 0.02*( fabs(scEta()) - 2.3);
  } 

}




