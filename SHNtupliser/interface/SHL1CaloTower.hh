#ifndef SHL1CALOTOWER_HH
#define SHL1CALOTOWER_HH

#include "TObject.h"

class SHL1CaloTower : public TObject {

public:
  enum ETTYPE{ECAL=0x1,HCAL=0x2,ECALHCAL=0x3};
  
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
  int etTS()const{return emEtTS()+hadEtTS();}
  int etTS(int etCode)const{return (etCode&ECAL)!=0 ? emEtTS() : 0 + (etCode&HCAL)!=0 ? hadEtTS() : 0;}

  bool ecalFG()const{ return ecalFG_;}
  bool hcalFG()const{ return hcalFG_;}
  int trigScale()const{ return trigScale_;}
  
  
  ClassDef(SHL1CaloTower,1)
};

#endif
