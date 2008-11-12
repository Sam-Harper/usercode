#ifndef SHTRIGINFO
#define SHTRIGINFO

//this is a small class which holds information about a certain trigger
//currently this information is limited to the p4 of objects which pass it

#include "TObject.h"
#include "TLorentzVector.h"

#include <vector>

class SHTrigInfo : public TObject {

private:
  std::vector<TLorentzVector> trigObjs_;
  int trigId_;
  

public:
  SHTrigInfo(int trigId=-1):trigObjs_(),trigId_(trigId){}
  ~SHTrigInfo(){}
  
  //modifiers
  void setTrigId(int trigId){trigId_=trigId;}
  void addObj(const TLorentzVector& p4){trigObjs_.push_back(p4);}
  void clear(){trigObjs_.clear();}
  
  //accessors
  int nrPass()const{return trigObjs_.size();}
  const TLorentzVector & getObjP4(int indx)const{return trigObjs_[indx];}
  int trigId()const{return trigId_;}

  //trigger pass functions

  //just asks is there an object which has passed the trigger in the event
  bool passTrig()const{return !trigObjs_.empty();}
  //does a deltaR matching on the objects p4 to see if there is a trigger object which matches it
  bool passTrig(const TLorentzVector& p4)const{return passTrig(p4.Eta(),p4.Phi());}
  bool passTrig(double eta,double phi)const;
  
 
  
  ClassDef(SHTrigInfo,1)
};

#endif
