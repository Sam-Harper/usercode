#ifndef SHPILEUPINFO
#define SHPILEUPINFO

#include "TObject.h"

class SHPileUpInfo: public TObject {
private:
  int bx_;
  int nrInteractions_;
  int nrTrueInteractions_; 

public:
  SHPileUpInfo();
  SHPileUpInfo(int iBx,int iNrInt,int iNrTrueInt):
    bx_(iBx),nrInteractions_(iNrInt),
    nrTrueInteractions_(iNrTrueInt) {}
  
  int bx()const{return bx_;}
  int nrInteractions()const{return nrInteractions_;}
  int nrTrueInteractions()const{return nrTrueInteractions_;}
  ClassDef(SHPileUpInfo,1)
     
};

#endif
