#ifndef SHPFCANDIDATE_HH
#define SHPFCANDIDATE_HH

#include "TObject.h"

//will ultimately expand but want  it to be light weight for now...
class SHPFCandidate {
  
private:
  float pt_,eta_,phi_,mass_;
  float mvaNothingGamma_;
  int scSeedCrysId_;
  float vx_,vy_,vz_;


public:
  SHPFCandidate():
    pt_(0.),eta_(0.),phi_(0.),mass_(0.),scSeedCrysId_(0){}
  SHPFCandidate(float iPt,float iEta,float iPhi,float iMass,float iMVANothingGamma,int iSCSeedCrysId):
    pt_(iPt),eta_(iEta),phi_(iPhi),mass_(iMass),mvaNothingGamma_(iMVANothingGamma),scSeedCrysId_(iSCSeedCrysId),vx_(0.),vy_(0.),vz_(0.){}
  
  float pt()const{return pt_;}
  float eta()const{return eta_;}
  float phi()const{return phi_;}
  float mass()const{return mass_;}
  
  void setVertex(float x,float y,float z){vx_=x;vy_=y;vz_=z;}

  ClassDef(SHPFCandidate,1)
};

#endif
