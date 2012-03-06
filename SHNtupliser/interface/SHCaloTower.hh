#ifndef SHCALOTOWER
#define SHCALOTOWER

#include "TObject.h"

class SHCaloTower : public TObject {
  
private:
  int towerId_;

  float emNrgy_;
  float hadD1Nrgy_;
  float hadD2Nrgy_;
  
  float eta_;
  float phi_;

public:
  SHCaloTower():towerId_(0),emNrgy_(0.),hadD1Nrgy_(0.),hadD2Nrgy_(0.),eta_(0.),phi_(0.){}
  SHCaloTower(int iId,float iEMNrgy,float iHadD1Nrgy,float iHadD2Nrgy,
	      float iEta,float iPhi);//iEta = input eta not iEta..
  

  float towerId()const{return towerId_;}
  float emNrgy()const{return emNrgy_;}
  float hadD1Nrgy()const{return hadD1Nrgy_;}
  float hadD2Nrgy()const{return hadD2Nrgy_;}
  float eta()const{return eta_;}
  float phi()const{return phi_;}

  ClassDef(SHCaloTower,1)
};

#endif
