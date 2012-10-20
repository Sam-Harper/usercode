#ifndef SHISOLTRACK
#define SHISOLTRACK

//this class is a basic track used for storing all the tracks near the electron
//it is intended to be very lightweight to save diskspace

#include "TVector3.h"

class SHIsolTrack : public TObject {

private:
  TVector3 p3_; //the momentum
  TVector3 vtxPos_; //the vtx position
  bool posCharge_; //if the charge is positive, new for v2
  int vertexNr_;
  float chi2_;
  int ndof_;

public:
  SHIsolTrack();
  SHIsolTrack(const TVector3& p3,const TVector3& vtxPos,bool posCharge,int vertexNr,float chi2,int ndof);
  
  const TVector3& p3()const{return p3_;}
  const TVector3& vtxPos()const{return vtxPos_;}
  int charge()const{return posCharge_ ? 1 : -1;}
  float dz()const;
  float pt()const{return p3().Pt();}
  int vertexNr()const{return vertexNr_;}
  int ndof()const{return ndof_;}
  float chi2()const{return chi2_;}

  ClassDef(SHIsolTrack,3)
};

#endif
