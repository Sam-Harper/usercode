#ifndef SHL1CAND
#define SHL1CAND

#include "TLorentzVector.h"
#include "TObject.h"

class SHL1Cand : public TObject {
private:
  TLorentzVector p4_;
  int type_;
public:
  SHL1Cand(){}
  SHL1Cand(const TLorentzVector& iP4,int iType):p4_(iP4),type_(iType){}
  ~SHL1Cand(){}

  const TLorentzVector& p4()const{return p4_;}
  int type()const{return type_;}

  ClassDef(SHL1Cand,1)
};

#endif
