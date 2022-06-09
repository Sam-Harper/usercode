#ifndef SHPFCANDIDATE_HH
#define SHPFCANDIDATE_HH

#include "TObject.h"
#include "TVector3.h"
#include <cmath>
#include <iostream>
//will ultimately expand but want  it to be light weight for now...
class SHPFCandidate {
  
private:
  float pt_,eta_,phi_,mass_;
  float mvaNothingGamma_;
  int scSeedCrysId_;
  float vx_,vy_,vz_;
  float hadNrgyRaw_;
  float hadNrgyMap_;
  float hadNrgyMapRaw_;
  float hcalClusterNrgy_;
  


public:
  SHPFCandidate():
    pt_(0.),eta_(0.),phi_(0.),mass_(0.),scSeedCrysId_(0){}
  SHPFCandidate(float iPt,float iEta,float iPhi,float iMass,
		float iMVANothingGamma,int iSCSeedCrysId,
		float iHadNrgyRaw=0,float iHadNrgyMap=0,float iHadNrgyMapRaw=0,
		float iHcalClusterNrgy=0):
    pt_(iPt),eta_(iEta),phi_(iPhi),mass_(iMass),
    mvaNothingGamma_(iMVANothingGamma),scSeedCrysId_(iSCSeedCrysId),
    vx_(0.),vy_(0.),vz_(0.),
    hadNrgyRaw_(iHadNrgyRaw),hadNrgyMap_(iHadNrgyMap),hadNrgyMapRaw_(iHadNrgyMapRaw),
    hcalClusterNrgy_(iHcalClusterNrgy){}
  virtual ~SHPFCandidate(){}
  float pt()const{return pt_;}
  float eta()const{return eta_;}
  float phi()const{return phi_;}
  float mass()const{return mass_;}
  float mvaNothingGamma()const{return mvaNothingGamma_;}
  int scSeedCrysId()const{return scSeedCrysId_;}
  float hadNrgyRaw()const{return hadNrgyRaw_;}
  float hadNrgyMap()const{return hadNrgyMap_;}
  float hadNrgyMapRaw()const{return hadNrgyMapRaw_;}
  float hcalClusterNrgy()const{return hcalClusterNrgy_;}

  float py()const{return pt_*std::sin(phi_);}
  float px()const{return pt_*std::cos(phi_);}
  float dxy(const TVector3& pos)const;
  float vx()const{return vx_;}
  float vy()const{return vy_;}
  float vz()const{return vz_;}

  void setVertex(float x,float y,float z){vx_=x;vy_=y;vz_=z;}

  std::ostream& print(std::ostream& out)const;
  friend std::ostream& operator<<(std::ostream& out,const SHPFCandidate& cand){return cand.print(out);}

  ClassDef(SHPFCandidate,4)
};

#endif
