#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"
 
float heep::Ele::scSigmaEtaEta()const
{
  if(fabs(scEta())<1.479) return clusShapeData_.sigmaEtaEta; //barrel case, no correction
  else{ //endcap, need to apply eta correction
    return clusShapeData_.sigmaEtaEta - 0.02*( fabs(scEta()) - 2.3);
  } 

}

