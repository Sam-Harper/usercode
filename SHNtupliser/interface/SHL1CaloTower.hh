#ifndef SHL1CALOTOWER
#define SHL1CALOTOWER

#include "TObject.h"

class SHL1CaloTower : public TObject {
  
private:
  int iEta_;
  int iPhi_;
  
  // ts = in triger scale, not in units of GeV
  int emEtTS_;
  int hadEtTS_;  
  bool ecalFG_;
  bool hcalFG_;
  int trigScale_;

public:
  SHL1CaloTower():iEta_(0),iPhi_(0),emEtTS_(0),hadEtTS_(0),ecalFG_(false),hcalFG_(false),trigScale_(0){}
  SHL1CaloTower(int iIEta,int iIPhi,int iEMEtTS,int iHadEtTS,bool iECALFG,bool iHCALFG,int iTrigScale=2);
  
  int iEta()const{ return iEta_;}
  int iPhi()const{ return iPhi_;}
  int emEtTS()const{ return emEtTS_;}
  int hadEtTS()const{ return hadEtTS_;}
  bool ecalFG()const{ return ecalFG_;}
  bool hcalFG()const{ return hcalFG_;}
  int trigScale()const{ return trigScale_;}
  
  ClassDef(SHL1CaloTower,1)
};

#endif
