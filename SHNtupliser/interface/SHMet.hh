#ifndef SHMET
#define SHMET


//a very basic class to hold the met information for the event

#include "SHarper/SHNtupliser/interface/MathFuncs.hh"

#include "TObject.h"


#include <cmath>

class SHMet : public TObject {

private:
  double mEx_;
  double mEy_;
  double emEtEB_;
  double emEtEE_;
  double emEtHF_;
  double hadEtHB_;
  double hadEtHE_; 
  double hadEtHO_;
  double hadEtHF_;
  

public:
  SHMet();
  //SHMet(double mEx,double mEy,double emEt,double hadEt);
  ~SHMet(){}

  double mEt()const{return std::sqrt(mEx_*mEx_+mEy_*mEy_);}
  double mEx()const{return mEx_;}
  double mEy()const{return mEy_;}
  double emEt()const{return emEtEB_+emEtEE_+emEtHF_;}
  double hadEt()const{return hadEtHB_+hadEtHE_+hadEtHO_+hadEtHF_;}
  double sumEt()const{return emEt()+hadEt();}
  double phi()const{return MathFuncs::phi(mEx_,mEy_);}

  void setMet(double mEx,double mEy);
  void setSumEmEt(double etEB,double etEE,double etHF);
  void setSumHadEt(double etHB,double etHE,double etHO,double etHF);

  void clear();

  ClassDef(SHMet,1)

};


#endif
