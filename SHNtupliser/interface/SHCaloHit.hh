#ifndef SHCALOHIT
#define SHCALOHIT

#include "TObject.h"

#include <iostream>


class SHCaloHit : public TObject {

 private:
  float nrgy_;
  int detId_;

 public:
  SHCaloHit():nrgy_(0.),detId_(0){}
  SHCaloHit(int id,float nrgy);
  SHCaloHit(const SHCaloHit & rhs);
  ~SHCaloHit(){}
  
  SHCaloHit& operator=(const SHCaloHit& rhs);

 

  float nrgy()const{return nrgy_;}
  int detId()const{return detId_;}

  void setNrgy(float nrgy){nrgy_=nrgy;}
  void setDetId(int detId){detId_=detId;}

  bool operator==(const SHCaloHit& rhs)const{return detId_==rhs.detId_;}
  
  ClassDef(SHCaloHit,1)

};


//stop gap to remain compatable with old code
typedef SHCaloHit SHEcalHit;
typedef SHCaloHit SHHcalHit;

#endif
