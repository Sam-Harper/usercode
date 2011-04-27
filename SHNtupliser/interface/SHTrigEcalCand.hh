#ifndef SHTRIGECALCAND
#define SHTRIGECALCAND

#include "TObject.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include <vector>
#include <string>
#include <iostream>

class SHTrigEcalCand : public TObject {

private:
  TLorentzVector p4_;
  
  float clusNrgy_;
  float preShowerNrgy_;
  TVector3 clusPos_;

  bool isIso_;
  
  std::vector<std::pair<std::string,float> > vars_; //a sorted vector containing all the extra debug variables

public: 
  SHTrigEcalCand():
    p4_(),clusNrgy_(-999),preShowerNrgy_(-999),clusPos_(),isIso_(false){}
  SHTrigEcalCand(const TLorentzVector& p4,float clusNrgy,float preNrgy,const TVector3& clusPos):
    p4_(p4),clusNrgy_(clusNrgy),preShowerNrgy_(preNrgy),clusPos_(clusPos),isIso_(false){}
  ~SHTrigEcalCand(){}

  void setVars(std::vector<std::pair<std::string,float> >& inputVar);//note, modifies its arguments
  
  const TLorentzVector& p4()const{return p4_;}
  float clusNrgy()const{return clusNrgy_;}
  float preShowerNrgy()const{return preShowerNrgy_;}
  const TVector3& clusPos()const{return clusPos_;}
  bool isIso()const{return isIso_;}
  float eta()const{return p4_.Eta();}
  float nrgy()const{return p4_.E();}
  float phi()const{return p4_.Phi();}
  float et()const{return p4_.Et();}
  
  float var(const std::string& varName)const;


  friend std::ostream &operator <<(std::ostream& output,const SHTrigEcalCand &cand);
  std::ostream &print(std::ostream& output)const;
  void print(){std::cout <<*this<<std::endl;}

  ClassDef(SHTrigEcalCand,1)

};


#endif
